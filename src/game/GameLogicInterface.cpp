
#include "game/GameLogicInterface.h"

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

    Texture tex(1920, 1080);

    float camZoom = 1.0f;
    float camX = 0.0f;
    float camY = 0.0f;

    int maxItter = 1000;
    float colorShiftFactor = 30.0f;


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

    void fillWithMandelbrotData(std::vector<std::array<float, 4>>& pixelData, int pixWide, float camX, float camY, float zoom) {

        for (int x = 0; x < pixWide; x++) {
            for (int y = 0; y < pixelData.size() / pixWide; y++) {

                double x0 = x / 1920.0;
                double y0 = y / 1080.0;

                x0 *= 3.5;
                x0 -= 2.5;
                y0 *= 2.0;
                y0 -= 1.0;

                x0 *= zoom;
                y0 *= zoom;

                x0 += camX;
                y0 += camY;

                float man = (float)mandelbrotAt(x0, y0, maxItter) / (float)maxItter;
                std::array<float, 3> color = colorRotator(man);

                pixelData[x + y * pixWide] = { color[0], color[1], color[2], 1.0f };

            }
        }

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
            "uniform float u_zoom;\n"
            "uniform vec2 u_manTrans;\n"
            "uniform int u_maxItter;\n"
            "uniform float u_colorShiftFactor;\n"
            "\n"
            "uniform sampler2D u_texture;\n"
            "\n"
            "void main()\n"
            "{\n"
            ""
            "   double x0 = v_texCoord[0] * 3.5f - 2.5f;\n"
            "   double y0 = v_texCoord[1] * 2.0f - 1.0f;\n"
            ""
            "   x0 *= u_zoom;\n"
            "   y0 *= u_zoom;\n"
            ""
            "   x0 += u_manTrans[0];\n"
            "   y0 += u_manTrans[1];\n"
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
            "   float colorShift = float(itter) / float(u_maxItter);"
            ""
            "   colorShift *= u_colorShiftFactor;;"
            "   float r = 1.0f - (cos(colorShift * 3.14159f * 1.0f) + 1.0f) / 2.0f;"
            "   float g = 1.0f - (cos(colorShift * 3.14159f * 3.0f) + 1.0f) / 2.0f;"
            "   float b = 1.0f - (cos(colorShift * 3.14159f * 6.0f) + 1.0f) / 2.0f;"
            ""
            ""
            "	color = vec4(r, g, b, 1.0f);\n"
            "};\n";

        static Shader sh = Shader(vertexShaderString, fragmentShaderString);

        sh.setUniform1f("u_zoom", camZoom);
        sh.setUniform2f("u_manTrans", camX, camY);
        sh.setUniform1i("u_maxItter", maxItter);
        sh.setUniform1f("u_colorShiftFactor", colorShiftFactor);

        static TexturedQuad gpuQuad;
        gpuQuad.setBounding(-2.0f, -1.0f, 4.0f, 2.0f);
        gpuQuad.setShader(sh);

        tex.bindAsRenderTarget();
        gpuQuad.render();
        tex.unbindAsRenderTarget();

    }

    void generateMandelbrot_cpu(Texture & texture) {
        std::vector<std::array<float, 4>> pixelData(1920 * 1080);
        fillWithMandelbrotData(pixelData, 1920, camX, camY, camZoom);

        tex.generateFromData(1920, 1080, &pixelData[0][0], pixelData.size());
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

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

    static TexturedQuad tq;
    tq.setTexture(tex);
    tq.setX(ViewportManager::getLeftViewportBound());
    tq.setY(ViewportManager::getBottomViewportBound());
    tq.setWidth(ViewportManager::getRightViewportBound() - ViewportManager::getLeftViewportBound());
    tq.setHeight(ViewportManager::getTopViewportBound() - ViewportManager::getBottomViewportBound());
    
    tq.render();


    bool wasMoved = false;

    if (window.keyIsDown(GLFW_KEY_E)) {
        camZoom *= 0.97f;

        camX += window.getMouseX() * camZoom * 0.05f;
        camY += window.getMouseY() * camZoom * 0.05f;

        wasMoved = true;
    }

    else if (window.keyIsDown(GLFW_KEY_Q)) {
        camZoom *= 1.03f;

        camX += window.getMouseX() * camZoom * 0.05f;
        camY += window.getMouseY() * camZoom * 0.05f;

        wasMoved = true;
    }

    if (window.keyIsDown(GLFW_KEY_W)) {
        camY += camZoom * 0.05f;
        wasMoved = true;
    }
    else if (window.keyIsDown(GLFW_KEY_A)) {
        camX -= camZoom * 0.05f;
        wasMoved = true;
    }
    if (window.keyIsDown(GLFW_KEY_S)) {
        camY -= camZoom * 0.05f;
        wasMoved = true;
    }
    else if (window.keyIsDown(GLFW_KEY_D)) {
        camX += camZoom * 0.05f;
        wasMoved = true;
    }

    if (window.keyIsDown(GLFW_KEY_O)) {
        camZoom *= 1.01f;
        wasMoved = true;
    } else if (window.keyIsDown(GLFW_KEY_P)) {
        camZoom *= 0.99f;
        wasMoved = true;
    }

    if (wasMoved) {
        if (renderWithGPU)
            generateMandelbrot_gpu(tex);
        else
            generateMandelbrot_cpu(tex);
    }

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
        tex.saveToFile("mandelbrot-image.png");
    }

    if (key == GLFW_KEY_1) {
        renderWithGPU = true;
    }

    if (key == GLFW_KEY_2) {
        renderWithGPU = false;
    }

}

void GameLogicInterface::characterCallback(unsigned int codepoint)
{
}
