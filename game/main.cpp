#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#define WIDTH 1280
#define HEIGHT 720

void init(void);
void createShape(void);
void render(void);
void compileShaders(char const *, char const *);
void addShader(GLuint, std::string const &, GLenum);
int readFile(char const *, std::string &);

/*
 *struct PersProjInfo
 *{
 *    float FOV;
 *    float Width; 
 *    float Height;
 *    float zNear;
 *    float zFar;
 *};
 *
 *PersProjInfo gPersProjInfo;
 *
 *class Camera
 *{
 *public:
 *
 *    Camera(int WindowWidth, int WindowHeight);
 *
 *    Camera(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up);
 *
 *    //bool OnKeyboard(OGLDEV_KEY Key);
 *
 *    void OnMouse(int x, int y);
 *
 *    void OnRender();
 *
 *    const glm::vec3& GetPos() const
 *    {
 *        return m_pos;
 *    }
 *
 *    const glm::vec3& GetTarget() const
 *    {
 *        return m_target;
 *    }
 *
 *    const glm::vec3& GetUp() const
 *    {
 *        return m_up;
 *    }
 *    
 *    //void AddToATB(TwBar* bar);
 *
 *private:
 *
 *    void Init();
 *    void Update();
 *
 *    glm::vec3 m_pos;
 *    glm::vec3 m_target;
 *    glm::vec3 m_up;
 *
 *    int m_windowWidth;
 *    int m_windowHeight;
 *
 *    float m_AngleH;
 *    float m_AngleV;
 *
 *    bool m_OnUpperEdge;
 *    bool m_OnLowerEdge;
 *    bool m_OnLeftEdge;
 *    bool m_OnRightEdge;
 *
 *    glm::vec2 m_mousePos;
 *};
 *
 *class Pipeline
 *{
 *public:
 *    Pipeline()
 *    {
 *        m_scale      = glm::vec3(1.0f, 1.0f, 1.0f);
 *        m_worldPos   = glm::vec3(0.0f, 0.0f, 0.0f);
 *        m_rotateInfo = glm::vec3(0.0f, 0.0f, 0.0f);
 *    }
 *
 *    void Scale(float s)
 *    {
 *        Scale(s, s, s);
 *    }
 *    
 *    
 *    void Scale(const glm::vec3& scale)
 *    {
 *        Scale(scale.x, scale.y, scale.z);
 *    }
 *    
 *    void Scale(float ScaleX, float ScaleY, float ScaleZ)
 *    {
 *        m_scale.x = ScaleX;
 *        m_scale.y = ScaleY;
 *        m_scale.z = ScaleZ;
 *    }
 *
 *    void WorldPos(float x, float y, float z)
 *    {
 *        m_worldPos.x = x;
 *        m_worldPos.y = y;
 *        m_worldPos.z = z;
 *    }
 *    
 *    void WorldPos(const glm::vec3& Pos)
 *    {
 *        m_worldPos = Pos;
 *    }
 *
 *    void Rotate(float RotateX, float RotateY, float RotateZ)
 *    {
 *        m_rotateInfo.x = RotateX;
 *        m_rotateInfo.y = RotateY;
 *        m_rotateInfo.z = RotateZ;
 *    }
 *    
 *    void Rotate(const glm::vec3& r)
 *    {
 *        Rotate(r.x, r.y, r.z);
 *    }
 *
 *    void SetPerspectiveProj(const PersProjInfo& p)
 *    {
 *        m_persProjInfo = p;
 *    }
 *    
 *    void SetCamera(const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up)
 *    {
 *        m_camera.Pos = Pos;
 *        m_camera.Target = Target;
 *        m_camera.Up = Up;
 *    }
 *    
 *    void SetCamera(const Camera& camera)
 *    {
 *        SetCamera(camera.GetPos(), camera.GetTarget(), camera.GetUp());
 *    }
 *    
 *    const glm::mat4& GetWPTrans();
 *    const glm::mat4& GetWVTrans();
 *    const glm::mat4& GetVPTrans();
 *    const glm::mat4& GetWVPTrans();
 *    const glm::mat4& GetWorldTrans();
 *    const glm::mat4& GetViewTrans();
 *    const glm::mat4& GetProjTrans();
 *
 *private:
 *    glm::vec3 m_scale;
 *    glm::vec3 m_worldPos;
 *    glm::vec3 m_rotateInfo;
 *
 *    PersProjInfo m_persProjInfo;
 *
 *    struct {
 *        glm::vec3 Pos;
 *        glm::vec3 Target;
 *        glm::vec3 Up;
 *    } m_camera;
 *
 *    glm::mat4 m_WVPtransformation;
 *    glm::mat4 m_VPtransformation;
 *    glm::mat4 m_WPtransformation;
 *    glm::mat4 m_WVtransformation;
 *    glm::mat4 m_Wtransformation;
 *    glm::mat4 m_Vtransformation;
 *    glm::mat4 m_ProjTransformation;
 *};
 *
 *const glm::mat4& Pipeline::GetProjTrans() 
 *{
 *    m_ProjTransformation.InitPersProjTransform(m_persProjInfo);
 *    return m_ProjTransformation;
 *}
 *
 *
 *const glm::mat4& Pipeline::GetVPTrans()
 *{
 *    GetViewTrans();
 *    GetProjTrans();
 *       
 *    m_VPtransformation = m_ProjTransformation * m_Vtransformation;
 *    return m_VPtransformation;
 *}
 *
 *const glm::mat4& Pipeline::GetWorldTrans()
 *{
 *    glm::mat4 ScaleTrans, RotateTrans, TranslationTrans;
 *
 *    ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
 *    RotateTrans.InitRotateTransform(m_rotateInfo.x, m_rotateInfo.y, m_rotateInfo.z);
 *    TranslationTrans.InitTranslationTransform(m_worldPos.x, m_worldPos.y, m_worldPos.z);
 *
 *    m_Wtransformation = TranslationTrans * RotateTrans * ScaleTrans;
 *    return m_Wtransformation;
 *}
 *
 *const glm::mat4& Pipeline::GetViewTrans()
 *{
 *    glm::mat4 CameraTranslationTrans, CameraRotateTrans;
 *
 *    CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
 *    CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);
 *    
 *    m_Vtransformation = CameraRotateTrans * CameraTranslationTrans;
 *
 *    return m_Vtransformation;
 *}
 *
 *const glm::mat4& Pipeline::GetWVPTrans()
 *{
 *    GetWorldTrans();
 *    GetVPTrans();
 *
 *    m_WVPtransformation = m_VPtransformation * m_Wtransformation;
 *    return m_WVPtransformation;
 *}
 *
 *
 *const glm::mat4& Pipeline::GetWVTrans()
 *{
 *        GetWorldTrans();
 *    GetViewTrans();
 *        
 *        m_WVtransformation = m_Vtransformation * m_Wtransformation;
 *        return m_WVtransformation;
 *}
 *
 *
 *const glm::mat4& Pipeline::GetWPTrans()
 *{
 *        glm::mat4 PersProjTrans;
 *
 *        GetWorldTrans();
 *        PersProjTrans.InitPersProjTransform(m_persProjInfo);
 *
 *        m_WPtransformation = PersProjTrans * m_Wtransformation;
 *        return m_WPtransformation;
 *}
 */
int main(void)
{
    GLFWwindow * window;

    if(! glfwInit()) {
        std::cerr << "Could not init GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "3D Pong", NULL, NULL);
    if(! window) {
        glfwTerminate();
        std::cerr << "Could not open window\n";
        return 1;
    }

    glfwMakeContextCurrent(window);
    if(gl3wInit()) {
        std::cerr << "Could not initialize OpenGL\n";
    }

    init();

    while(! glfwWindowShouldClose(window)) {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

GLuint vbo;
GLuint ibo;
GLuint vao;
GLint world_location;

void init(void)
{
    createShape();
    compileShaders("main.vert", "main.frag");

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void createShape()
{
    Vertex vertices[4];
    vertices[0].position = glm::vec3(-1.0f, -1.0f, 0.0f);
    vertices[1].position = glm::vec3(0.0f, -1.0f, 1.0f);
    vertices[2].position = glm::vec3(1.0f, -1.0f, 0.0f);
    vertices[3].position = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[0].color = glm::vec3(1.0f, 1.0f, 0.0f);
    vertices[1].color = glm::vec3(1.0f, 0.0f, 0.0f);
    vertices[2].color = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices[3].color = glm::vec3(0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //unsigned int indices[] = { 0, 1, 3,
                               //0, 1, 2 };

    unsigned int indices[] = { 0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2 };

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

float rot = 0.1f;

void render(void)
{
    rot += 0.01f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    glm::mat4 mvp = perspective * view * translate * rotate * scale;

    glUniformMatrix4fv(world_location, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void compileShaders(char const * vert_file, char const * frag_file)
{
    GLuint shader_program = glCreateProgram();

    if(! shader_program) {
        std::cerr << "Error creating shader program\n";
        exit(1);
    }

    std::string vs, fs;

    if (! readFile(vert_file, vs)) {
        exit(1);
    };

    if (! readFile(frag_file, fs)) {
        exit(1);
    };

    addShader(shader_program, vs.c_str(), GL_VERTEX_SHADER);
    addShader(shader_program, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint success = 0;
    GLchar error_log[1024] = {0};

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(! success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        std::cerr << "Error linking shader program: " << error_log << "\n";
        exit(1);
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if(! success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        std::cerr << "Error compiling shader type: " << error_log << "\n";
        exit(1);
    }

    glUseProgram(shader_program);

    world_location = glGetUniformLocation(shader_program, "transform");
    assert(world_location != 0xFFFFFFFF);
}

void addShader(GLuint shader_program, std::string const & shader_text, GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);

    if(! shader_obj) {
        std::cerr << "Error creating shader type " << shader_type << "\n";
        exit(1);
    }

    GLchar const * p[1];
    p[0] = shader_text.c_str();
    GLint lengths[1];
    lengths[0] = shader_text.length();
    glShaderSource(shader_obj, 1, p, lengths);
    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    if(! success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
        std::cerr << "Error compiling shader type " << shader_type << ": " << info_log << "\n";
        exit(1);
    }

    glAttachShader(shader_program, shader_obj);
}

int readFile(char const * file, std::string & data)
{
    if(std::ifstream is{file, std::ios::ate}) {
        auto size = is.tellg();
        std::string str(size, '\0');
        is.seekg(0);
        is.read(&str[0], size);
        data = str;
    } else {
        std::cerr << "Could not open " << file << "\n";
        return 0;
    }

    return 1;
}
