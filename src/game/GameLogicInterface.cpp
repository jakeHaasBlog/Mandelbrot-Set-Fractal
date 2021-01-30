
#include "game/GameLogicInterface.h"

#include <string>

// -------------------------------- The Mandelbrot Algorithm Psudocode ---------------------------------------------
/*
* for each pixel (Px, Py) on the screen do
    x0 := scaled x coordinate of pixel (scaled to lie in the Mandelbrot X scale (-2.5, 1))
    y0 := scaled y coordinate of pixel (scaled to lie in the Mandelbrot Y scale (-1, 1))
    x := 0.0
    y := 0.0
    iteration := 0
    max_iteration := 1000
    while (x*x + y*y ≤ 2*2 AND iteration < max_iteration) do
        xtemp := x*x - y*y + x0
        y := 2*x*y + y0
        x := xtemp
        iteration := iteration + 1
    
    color := palette[iteration]
    plot(Px, Py, color)
*/

// this stops the variables declared here from becoming globaly accessable
namespace {

    bool renderWithGPU = true;

    Texture tex(1080, 720);

    double camZoom = 1.0f;
    double camX = -0.5f;
    double camY = 0.0f;

    int maxItter = 300;
    float colorShiftFactor = 2.0f;

    bool rerender = true;
    bool saveFlag = false;

    std::array<float, 3> colorRotator(float colorShift) {

        colorShift *= colorShiftFactor;
        
        float r = 1.0f - (cos(colorShift * 3.14159f * 1.0f) + 1.0f) / 2.0f;
        float g = 1.0f - (cos(colorShift * 3.14159f * 3.0f) + 1.0f) / 2.0f;
        float b = 1.0f - (cos(colorShift * 3.14159f * 5.0f) + 1.0f) / 2.0f;
        
        return { r, g, b };

    }

    int mandelbrotAt(double x, double y, int maxItter) {
        double x0 = x;
        double y0 = y;

        double x1 = 0, y1 = 0;
        int itter = 0;

        while (x1 * x1 + y1 * y1 <= 2*2 && itter < maxItter) {
            double xTemp = (x1 * x1) - (y1 * y1) + x0;
            y1 = 2 * x1 * y1 + y0;
            x1 = xTemp;
            itter++;
        }

        return itter;

    }

    void generateMandelbrot_gpu(Texture& texture) {

        static std::string vertexShaderString =
            "#version 330 core\n"
            "\n"
            "layout(location = 0) in vec2 position;\n"
            "layout(location = 1) in vec2 uvCoord;\n"
            "\n"
            "uniform vec2 u_stretch;\n"
            "uniform vec2 u_translation;\n"
            "uniform float u_aspectRatio;\n"
            "\n"
            "uniform vec2 u_texture_stretch;\n"
            "uniform vec2 u_texture_translation;\n"
            "\n"
            "out vec2 v_texCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "	gl_Position = vec4((position[0] / u_aspectRatio) * u_stretch[0] + (u_translation[0] / u_aspectRatio), position[1] * u_stretch[1] + u_translation[1], 0, 1);\n"
            "	v_texCoord = vec2(uvCoord[0] * u_texture_stretch[0] + u_texture_translation[0], uvCoord[1] * u_texture_stretch[1] + u_texture_translation[1]);\n"
            "};\n";

        static std::string fragmentShaderString =
            "#version 330 core\n"
            "#extension GL_ARB_gpu_shader_fp64 : enable\n"
            "\n"
            "layout(location = 0) out vec4 color;\n"
            "\n"
            "in vec2 v_texCoord;\n"
            "\n"
            "uniform uvec2 u_zoom2i;\n"
            "uniform uvec2 u_manTransX2i;\n"
            "uniform uvec2 u_manTransY2i;\n"
            "uniform int u_maxItter;\n"
            "uniform float u_colorShiftFactor;\n"
            "\n"
            "uniform sampler2D u_texture;\n"
            "\n"
            "void main()\n"
            "{\n"
            ""
            "   double u_zoom = packDouble2x32(u_zoom2i);\n"
            "   double u_manTransX = packDouble2x32(u_manTransX2i);\n"
            "   double u_manTransY = packDouble2x32(u_manTransY2i);\n"
            ""
            "   double x0 = v_texCoord[0] * 3.5f - 1.75f;\n"
            "   double y0 = v_texCoord[1] * 2.0f - 1.0f;\n"
            ""
            "   x0 *= u_zoom;\n"
            "   y0 *= u_zoom;\n"
            ""
            "   x0 += u_manTransX;\n"   
            "   y0 += u_manTransY;\n"
            ""
            "   double x = 0.0f;\n"
            "   double y = 0.0f;\n"
            ""
            "   int itter = 0;\n"
            ""
            "   while (x * x + y * y <= 2 * 2 && itter < u_maxItter) {\n"
            "       double xtemp = x * x - y * y + x0;\n"
            "       y = 2 * x * y + y0;\n"
            "       x = xtemp;\n"
            "       itter = itter + 1;\n"
            "   }\n"
            ""
            "   float colorShift = float(itter) / float(u_maxItter);\n"
            ""
            "   colorShift *= u_colorShiftFactor;\n"
            "   float r = 1.0f - (cos(colorShift * 3.14159f * 1.0f) + 1.0f) / 2.0f;\n"
            "   float g = 1.0f - (cos(colorShift * 3.14159f * 3.0f) + 1.0f) / 2.0f;\n"
            "   float b = 1.0f - (cos(colorShift * 3.14159f * 5.0f) + 1.0f) / 2.0f;\n"
            ""
            ""
            "	color = vec4(r, g, b, 1.0f);\n"
            "};\n";

        static Shader sh = Shader(vertexShaderString, fragmentShaderString);

        sh.setUniform1i("u_maxItter", maxItter);
        sh.setUniform1f("u_colorShiftFactor", colorShiftFactor);

        // doubles cant be sent as uniforms so they are unpacked into 2 unsigned ints and reassembled in the shader, zoom and translation must have double precision
        unsigned int zoom[2];
        *((double*)(&zoom[0])) = camZoom;
        sh.setUniform2ui("u_zoom2i", zoom[0], zoom[1]);

        unsigned int manTransX[2];
        *((double*)(&manTransX[0])) = camX;
        sh.setUniform2ui("u_manTransX2i", manTransX[0], manTransX[1]);

        unsigned int manTransY[2];
        *((double*)(&manTransY[0])) = camY;
        sh.setUniform2ui("u_manTransY2i", manTransY[0], manTransY[1]);


        static TexturedQuad gpuQuad;
        gpuQuad.setShader(sh);

        texture.bindAsRenderTarget();
        gpuQuad.setX(ViewportManager::getLeftViewportBound());
        gpuQuad.setY(ViewportManager::getBottomViewportBound());
        gpuQuad.setWidth(ViewportManager::getRightViewportBound() - ViewportManager::getLeftViewportBound());
        gpuQuad.setHeight(ViewportManager::getTopViewportBound() - ViewportManager::getBottomViewportBound());
        gpuQuad.render();
        texture.unbindAsRenderTarget();

    }

    void generateMandelbrot_cpu(Texture & texture) {
        std::vector<std::array<float, 4>> pixelData(texture.getWidth() * texture.getHeight());
        //fillWithMandelbrotData(pixelData, 1920, camX, camY, camZoom);

        for (int x = 0; x < texture.getWidth(); x++) {
            for (int y = 0; y < pixelData.size() / texture.getWidth(); y++) {

                double x0 = (double)x / texture.getWidth();
                double y0 = (double)y / texture.getHeight();

                x0 *= 3.5;
                x0 -= 1.75;
                y0 *= 2.0;
                y0 -= 1.0;

                x0 *= camZoom;
                y0 *= camZoom;

                x0 += camX;
                y0 += camY;

                float man = (float)mandelbrotAt(x0, y0, maxItter) / (float)maxItter;
                std::array<float, 3> color = colorRotator(man);

                pixelData[x + y * texture.getWidth()] = { color[0], color[1], color[2], 1.0f };

            }
        }

        texture.generateFromData(texture.getWidth(), texture.getHeight(), &pixelData[0][0], pixelData.size());
    }
	
}

void GameLogicInterface::init() {
	window.setResolution(1920, 1080);

    if (renderWithGPU)
        generateMandelbrot_gpu(tex);
    else
        generateMandelbrot_cpu(tex);

}

// deltaTime is the milliseconds between frames. Use this for calculating movement to avoid slowing down if there is lag 
void GameLogicInterface::update(float deltaTime) {

    if (saveFlag) {
        Texture newT = Texture(3840, 2160);
        generateMandelbrot_gpu(newT);
        newT.saveToFile("mandelbrot-image(4k).png");
        saveFlag = false;
    }

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

    static TexturedQuad tq;
    tq.setTexture(tex);
    tq.setX(ViewportManager::getLeftViewportBound());
    tq.setY(ViewportManager::getBottomViewportBound());
    tq.setWidth(ViewportManager::getRightViewportBound() - ViewportManager::getLeftViewportBound());
    tq.setHeight(ViewportManager::getTopViewportBound() - ViewportManager::getBottomViewportBound());
    
    tq.render();


    if (window.keyIsDown(GLFW_KEY_W)) {
        camY += camZoom * 0.05 * ((double)deltaTime / 16.0);
        rerender = true;
    }
    if (window.keyIsDown(GLFW_KEY_A)) {
        camX -= camZoom * 0.05 * ((double)deltaTime / 16.0);
        rerender = true;
    }
    if (window.keyIsDown(GLFW_KEY_S)) {
        camY -= camZoom * 0.05 * ((double)deltaTime / 16.0);
        rerender = true;
    }
    if (window.keyIsDown(GLFW_KEY_D)) {
        camX += camZoom * 0.05 * ((double)deltaTime / 16.0);
        rerender = true;
    }

    if (window.keyIsDown(GLFW_KEY_O)) {
        camZoom += (1.01 * camZoom - camZoom) * ((double)deltaTime / 16.0);
        rerender = true;
    } else if (window.keyIsDown(GLFW_KEY_P)) {
        camZoom -= (1.01 * camZoom - camZoom) * ((double)deltaTime / 16.0);
        rerender = true;
    }

    if (rerender) {
        if (renderWithGPU)
            generateMandelbrot_gpu(tex);
        else
            generateMandelbrot_cpu(tex);

        rerender = false;
    }


    std::string itterTxt = "Process Itterations: ";
    itterTxt.append(std::to_string(maxItter));

    static BitmapText maxItterCounter;
    maxItterCounter.setText(itterTxt);
    maxItterCounter.setPosition(ViewportManager::getLeftViewportBound(), ViewportManager::getTopViewportBound() - 0.08f);
    maxItterCounter.setCharHeight(0.08f);
    maxItterCounter.setColor(1, 1, 1);
    maxItterCounter.render();


    char colorShiftText[100];
    sprintf_s(colorShiftText, 100, "Color Shift Factor: %.0f", colorShiftFactor);

    static BitmapText colorShiftCounter;
    colorShiftCounter.setText(colorShiftText);
    colorShiftCounter.setPosition(ViewportManager::getLeftViewportBound(), ViewportManager::getTopViewportBound() - 0.08f * 2);
    colorShiftCounter.setCharHeight(0.08f);
    colorShiftCounter.setColor(1, 1, 1);
    colorShiftCounter.render();


    char posText[100];
    sprintf_s(posText, 100, "Pos(%.5f, %.5f)", camX, camY);

    static BitmapText posDisplay;
    posDisplay.setText(posText);
    posDisplay.setPosition(ViewportManager::getLeftViewportBound(), ViewportManager::getTopViewportBound() - 0.08f * 3);
    posDisplay.setCharHeight(0.06f);
    posDisplay.setColor(1, 1, 1);
    posDisplay.render();


    char zoomText[100];
    sprintf_s(zoomText, 100, "Zoom: %f", 1.0 / camZoom);

    static BitmapText zoomDisplay;
    zoomDisplay.setText(zoomText);
    zoomDisplay.setPosition(ViewportManager::getLeftViewportBound(), ViewportManager::getTopViewportBound() - 0.08f * 4);
    zoomDisplay.setCharHeight(0.06f);
    zoomDisplay.setColor(1, 1, 1);
    zoomDisplay.render();
   
}

void GameLogicInterface::cleanup() {

}

void GameLogicInterface::mouseMoveCallback(double xPos, double yPos)
{

}

void GameLogicInterface::mouseButtonCallback(int button, int action, int mods)
{

}

void GameLogicInterface::keyCallback(int key, int scancode, int action, int mods)
{
    
    if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL)) {
        saveFlag = true;
    }

    if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
        maxItter -= 10;
        rerender = true;
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        maxItter += 10;
        rerender = true;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        renderWithGPU = true;
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        renderWithGPU = false;
    }

    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        colorShiftFactor -= 1;
        rerender = true;
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        colorShiftFactor += 1;
        rerender = true;
    }


    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        camX += window.getMouseX() * camZoom;
        camY += window.getMouseY() * camZoom;

        camZoom *= 0.4;

        rerender = true;
    }

    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        camX -= window.getMouseX() * camZoom;
        camY -= window.getMouseY() * camZoom;

        camZoom *= 1.6;

        rerender = true;
    }

}

void GameLogicInterface::characterCallback(unsigned int codepoint)
{
}
