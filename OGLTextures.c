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

	adakanFungsi(glUniform1i);
	adakanFungsi(glGenBuffers);
	adakanFungsi(glBindBuffer);
	adakanFungsi(glBufferData);
	adakanFungsi(glUseProgram);
	adakanFungsi(glLinkProgram);
	adakanFungsi(glGetShaderiv);
	adakanFungsi(glGetProgramiv);
	adakanFungsi(glCreateShader);
	adakanFungsi(glShaderSource);
	adakanFungsi(glAttachShader);
	adakanFungsi(glDeleteShader);
	adakanFungsi(glDeleteProgram);
	adakanFungsi(glCompileShader);
	adakanFungsi(glCreateProgram);
	adakanFungsi(glActiveTexture);
	adakanFungsi(glValidateProgram);
	adakanFungsi(glGetShaderInfoLog);
	adakanFungsi(glGetProgramInfoLog);
	adakanFungsi(glGetUniformLocation);
	adakanFungsi(glVertexAttribPointer);
	adakanFungsi(glEnableVertexAttribArray);

	glViewport(0, 0, 640, 480);

	UpdateWindow(window);
	ShowWindow(window, SW_NORMAL);


	static const GLfloat vertex[20] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	COLORREF *grafis = malloc(sizeof(COLORREF) * 640 * 480);

	for (int i = 0; i < 640 * 480; ++i) {
		grafis[i] = (RGB(0, 100, 255)|(((BYTE)100)<<24));
	}

	GLuint tex[2];
	glGenTextures(2, tex);

	unsigned int panjang, lebar;

	panjang = 640;
	lebar = 480;

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, panjang, lebar, 0, GL_RGBA, GL_UNSIGNED_BYTE, grafis);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	for (int i = 0; i < 100 * 100; ++i) {
		grafis[i % 100 + (i / 100 * 640)] = (RGB(100, 0, 0)|(((BYTE)50)<<24));
	}

	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, panjang, lebar, 0, GL_RGBA, GL_UNSIGNED_BYTE, grafis);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(grafis);

	const char *vertexSource = 
		"#version 330 core \n"
		" \n"
		"layout(location = 0) in vec3 posisi; \n"
		"layout(location = 1) in vec2 coord; \n"
		"out vec2 texCoord; \n"
		" \n"
		"void main() { \n"
		"	gl_Position = vec4(posisi, 1.0); \n"
		"	texCoord = coord; \n"
		"}";

	const char *fragmentSource = 
		"#version 330 core \n"
		" \n"
		"in vec2 texCoord; \n"
		"uniform sampler2D back; \n"
		"uniform sampler2D tex; \n"
		" \n"
		"void main() { \n"
		"	vec4 color = texture(tex, texCoord); \n"
		"	gl_FragColor = vec4(color.rgb, color.a * 2.55); \n"
		// "	gl_FragColor = mix(texture(back, texCoord), color, color.a * 2.55); \n"
		"}";

	program = createShader(vertexSource, fragmentSource);
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "back"), 0);
	glUniform1i(glGetUniformLocation(program, "tex"), 1);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (void*)(3 * sizeof(GL_FLOAT)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) >= 0 && IsWindow(window)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		dc = BeginPaint(window, &ps);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		SwapBuffers(dc);

		EndPaint(window, &ps);
	}

	glDeleteProgram(program);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	UnregisterClass("ClassWindowGL", GetModuleHandle(NULL));
	return 0;
}