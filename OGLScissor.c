#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

#define adakanFungsi(fungsi) fungsi = loadFungsiOGL(#fungsi);

PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLACTIVETEXTUREARBPROC glActiveTexture;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

HMODULE ogllib = NULL;

void *loadFungsiOGL(const char *name) {
	void *p = (void *)wglGetProcAddress(name);
	if ((p == 0) || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
		if (ogllib == NULL) ogllib = LoadLibrary("opengl32.dll");
		p = (void*)GetProcAddress(ogllib, name);
	}

	return p;
}

static GLuint compileShader(GLenum tipe, const char *source) {
	GLuint shader;

	shader = glCreateShader(tipe);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint lewat;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &lewat);
	if (!lewat) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Error compile shader \n %s\n", infoLog);
	}

	return shader;
}

static GLuint createShader(const char *vertexSource, const char* fragmentSource) {
	GLuint program, vertex, fragment;

	program = glCreateProgram();
	vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
	fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	GLint lewat;
	GLchar infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &lewat);
	if (!lewat) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("Error Program\n %s\n", infoLog);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

int main() {
	WNDCLASS wc;
	HWND window;
	HDC dc;
	PAINTSTRUCT ps;
	MSG msg;
	GLuint program;

	memset(&wc, 0, sizeof(WNDCLASS));
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "ClassWindowGL";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	window = CreateWindow("ClassWindowGL", "Mencoba GL", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
		CW_USEDEFAULT, 656, 519, NULL, NULL, GetModuleHandle(NULL), NULL);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 
		1, 
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER, 
		PFD_TYPE_RGBA, 
		16, 
		0, 0, 0, 0, 0, 0, 
		0, 
		0, 
		0, 
		0, 0, 0, 0, 
		16, 
		8, 
		0, 
		PFD_MAIN_PLANE, 
		0, 
		0, 0, 0
	};

	dc = GetDC(window);
	GLuint pixelFormat = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixelFormat, &pfd);

	HGLRC hRC = wglCreateContext(dc);
	wglMakeCurrent(dc, hRC);

	glViewport(0, 0, 640, 480);

	UpdateWindow(window);
	ShowWindow(window, SW_NORMAL);

	const char *vSource = 
		"#version 330 core \n"
		" \n"
		"in vec2 posisi; \n"
		" \n"
		"void main() { \n"
		"	gl_Position = vec4(posisi, 0.0f, 1.0f); \n"
		"}";

	const char *fSource = 
		"#version 330 core \n"
		" \n"
		"void main() { \n"
		"	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
		"}";


	program = createShader(vSource, fSource);

	static const GLfloat vertex[6] = {
		0.0f, -0.5f, 
		0.5f, 0.5f, 
		-0.5f, 0.5f
	};

	GLint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, 0);

	glUseProgram(program);

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) >= 0 && IsWindow(window)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		dc = BeginPaint(window, &ps);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_SCISSOR_TEST);
		glScissor(280, 200, 80, 80);
		glViewport(280, 200, 80, 80);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisable(GL_SCISSOR_TEST);

		SwapBuffers(dc);

		EndPaint(window, &ps);
	}

	glDeleteProgram(program);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	UnregisterClass("ClassWindowGL", GetModuleHandle(NULL));
	return 0;
}