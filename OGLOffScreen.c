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
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLBINDFRAMEBUFFERPROC glDeleteFramebuffers;
PFNGLACTIVETEXTUREARBPROC glActiveTexture;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;


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

void offscreenRendering(GLuint texture) {
	const char *vSource = 
		"#version 330 core"
		" \n"
		"in vec2 posisi; \n"
		" \n"
		"void main() { \n"
		"	gl_Position = vec4(posisi, 0.0, 1.0); \n"
		"}";

	const char *fSource = 
		"#version 330 core"
		" \n"
		"void main() { \n"
		"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
		"}";

	GLuint frame, buffer, program, posID;

	static const GLfloat vertex[8] = {
		-1.0f, -1.0f, 
		-0.5f,  0.5f, 
		 0.5f, -0.5f, 
		 1.0f,  1.0f
	};

	glViewport(0, 0, 320, 240);

	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	program = createShader(vSource, fSource);
	glUseProgram(program);

	posID = glGetAttribLocation(program, "posisi");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glVertexAttribPointer(posID, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, 0);

	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(posID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(posID);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &frame);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(program);
}

int main() {
	WNDCLASS wc;
	HWND window;
	HDC dc;
	PAINTSTRUCT ps;
	MSG msg;
	GLuint program, vertexBuffer, texture, posID, texID, barisan;
	COLORREF *grafis;

	static const GLfloat vertex[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0.0f, 1.0f, 
		 0.5f, -0.5f, 1.0f, 0.0f, 
		 0.5f,  0.5f, 1.0f, 1.0f
	};

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
	adakanFungsi(glDeleteBuffers);
	adakanFungsi(glDeleteProgram);
	adakanFungsi(glCompileShader);
	adakanFungsi(glCreateProgram);
	adakanFungsi(glActiveTexture);
	adakanFungsi(glGenFramebuffers);
	adakanFungsi(glBindFramebuffer);
	adakanFungsi(glDeleteFramebuffers);
	adakanFungsi(glGenVertexArrays);
	adakanFungsi(glBindVertexArray);
	adakanFungsi(glValidateProgram);
	adakanFungsi(glGetShaderInfoLog);
	adakanFungsi(glGetAttribLocation);
	adakanFungsi(glGetProgramInfoLog);
	adakanFungsi(glGetUniformLocation);
	adakanFungsi(glVertexAttribPointer);
	adakanFungsi(glFramebufferTexture2D);
	adakanFungsi(glEnableVertexAttribArray);
	adakanFungsi(glDisableVertexAttribArray);

	UpdateWindow(window);
	ShowWindow(window, SW_NORMAL);

	const char *vertexS = 
		"#version 330 core \n"
		" \n"
		"in vec2 posisi; \n"
		"in vec2 tCoord; \n"
		"out vec2 texCoord; \n"
		" \n"
		"void main() { \n"
		"	gl_Position = vec4(posisi.x, posisi.y * -1, 0.0, 1.0); \n"
		"	texCoord = tCoord; \n"
		"}";

	const char *fragmentS = 
		"#version 330 core \n"
		" \n"
		"in vec2 texCoord; \n"
		" \n"
		"uniform sampler2D tex; \n"
		" \n"
		"void main() { \n"
		"	gl_FragColor = texture(tex, texCoord); \n"
		"}";

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	offscreenRendering(texture);

	glViewport(0, 0, 640, 480);

	program = createShader(vertexS, fragmentS);
	glUseProgram(program);

	posID = glGetAttribLocation(program, "posisi");
	texID = glGetAttribLocation(program, "tCoord");

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glGenVertexArrays(1, &barisan);
	glBindVertexArray(barisan);

	glVertexAttribPointer(posID, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
	glVertexAttribPointer(texID, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)(2 * sizeof(GL_FLOAT)));

	glEnableVertexAttribArray(posID);
	glEnableVertexAttribArray(texID);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) >= 0 && IsWindow(window)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		dc = BeginPaint(window, &ps);

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(barisan);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		SwapBuffers(dc);

		EndPaint(window, &ps);
	}

	return 0;
}