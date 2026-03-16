#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

/* ======================= ОБЩИЕ НАСТРОЙКИ ======================= */
int WinWidth = 1280, WinHeight = 720;
int selectedProgram = 0; /* 1 - fireworks, 2 - cube */

/* ======================= ФЕЙЕРВЕРКИ (FIREWORKS) ======================= */
#define MAX_PARTICLES 2000
#define PI 3.1415926f

const float GRAVITY = 400.0f;
const float MAX_RADIUS = 30.0f;

typedef enum {
    PARTICLE_SHELL,
    PARTICLE_FRAGMENT
} ParticleType;

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    float life;
    int alive;
    ParticleType type;
} Particle;

Particle particles[MAX_PARTICLES];
int numParticles = 0;
float spawnTimer = 0.0f;

void removeParticle(int index) {
    particles[index] = particles[numParticles - 1];
    numParticles--;
}

void spawnShell(float x, float y) {
    if (numParticles >= MAX_PARTICLES) return;
    Particle* p = &particles[numParticles++];
    p->x = x;
    p->y = y;
    p->vx = 0.0f;
    p->vy = 500.0f + rand() % 100;
    p->radius = 4.0f;
    p->life = 30.0f;
    p->alive = 1;
    p->type = PARTICLE_SHELL;
}

void spawnFragment(float x, float y, float vx, float vy) {
    if (numParticles >= MAX_PARTICLES) return;
    Particle* p = &particles[numParticles++];
    p->x = x;
    p->y = y;
    p->vx = vx;
    p->vy = vy;
    p->radius = 2.0f;
    p->life = 1.2f;
    p->alive = 1;
    p->type = PARTICLE_FRAGMENT;
}

void explodeShell(int index) {
    Particle* p = &particles[index];
    int count = 100 + rand() % 20;
    int i;
    for (i = 0; i < count; ++i) {
        float angle = (float)i / count * 2.0f * PI;
        float speed = 150.0f + rand() % 80;
        spawnFragment(p->x, p->y, cosf(angle) * speed, sinf(angle) * speed);
    }
    removeParticle(index);
}

void stepFireworks(float dt) {
    int i = 0;
    while (i < numParticles) {
        Particle* p = &particles[i];
        if (p->alive == 0) {
            i++;
            continue;
        }
        p->vy -= GRAVITY * dt;
        p->x += p->vx * dt;
        p->y += p->vy * dt;
        p->life -= dt;
        if (p->type == PARTICLE_SHELL) {
            p->radius += 8.0f * dt;
            if (p->radius >= MAX_RADIUS || p->life <= 0.0f || p->vy <= 0) {
                explodeShell(i);
                continue;
            }
        } else {
            if (p->life <= 0.0f) {
                removeParticle(i);
                continue;
            }
        }
        i++;
    }
}

/* ======================= КУБИК (ROTATING SQUARE) ======================= */
float squareSize = 50.0f;
float cubePosX = 256.0f, cubePosY = 256.0f;
float speedX = 3.0f, speedY = 2.5f;
float angle = 0.0f;
float rotationSpeed = 1.0f;
float rotationDecay = 0.99f;
float maxRotationSpeed = 10.0f;
float bounceRotationBoost = 2.0f;
int cubeDelay = 16;
long lastCubeTime = 0;

void stepCube() {
    long currentTime = clock();
    if (currentTime - lastCubeTime < cubeDelay) return;
    lastCubeTime = currentTime;

    cubePosX += speedX;
    cubePosY += speedY;

    float halfSize = squareSize / 2.0f;
    int bounced = 0; /* вместо bool */

    if (cubePosX + halfSize > WinWidth || cubePosX - halfSize < 0) {
        speedX = -speedX;
        bounced = 1;
        if (cubePosX + halfSize > WinWidth) cubePosX = WinWidth - halfSize;
        if (cubePosX - halfSize < 0) cubePosX = halfSize;
    }

    if (cubePosY + halfSize > WinHeight || cubePosY - halfSize < 0) {
        speedY = -speedY;
        bounced = 1;
        if (cubePosY + halfSize > WinHeight) cubePosY = WinHeight - halfSize;
        if (cubePosY - halfSize < 0) cubePosY = halfSize;
    }

    if (bounced) {
        rotationSpeed += bounceRotationBoost;
        if (rotationSpeed > maxRotationSpeed) rotationSpeed = maxRotationSpeed;
    }

    rotationSpeed *= rotationDecay;
    if (rotationSpeed < 0.5f) rotationSpeed = 0.5f;

    angle += rotationSpeed;
    if (angle > 360.0f) angle -= 360.0f;
}

/* ======================= ОБЩИЙ DISPLAY ======================= */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (selectedProgram == 1) { /* Fireworks */
        int i, j;
        for (i = 0; i < numParticles; ++i) {
            Particle* p = &particles[i];
            if (!p->alive) continue;

            if (p->type == PARTICLE_SHELL)
                glColor3f(1.0f, 0.0f, 0.0f);
            else
                glColor3f(1.0f, 0.4f, 0.2f);

            glBegin(GL_POLYGON);
            for (j = 0; j < 50; j++) {
                float angle = 2.0f * PI * (float)j / 50.0f;
                float dx = p->radius * cosf(angle);
                float dy = p->radius * sinf(angle);
                glVertex2f(p->x + dx, p->y + dy);
            }
            glEnd();
        }
    }
    else if (selectedProgram == 2) { /* Rotating cube */
        glPushMatrix();
        glTranslatef(cubePosX, cubePosY, 0.0f);
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        glColor3ub(255, 0, 0);
        glBegin(GL_QUADS);
        float half = squareSize / 2.0f;
        glVertex2f(-half, -half);
        glVertex2f(-half, half);
        glVertex2f(half, half);
        glVertex2f(half, -half);
        glEnd();
        glPopMatrix();
    }

    glutSwapBuffers();
}

/* ======================= ОБЩИЙ TIMER ======================= */
void timer(int value) {
    if (selectedProgram == 1) {
        const float dt = 1.0f / 60.0f;
        spawnTimer += dt;
        if (spawnTimer >= 1.0f) {
            float x = 50.0f + rand() % (WinWidth - 100);
            spawnShell(x, 30.0f);
            spawnTimer = 0.0f;
        }
        stepFireworks(dt);
    }
    else if (selectedProgram == 2) {
        stepCube();
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

/* ======================= RESHAPE ======================= */
void reshape(GLint w, GLint h) {
    WinWidth = w;
    WinHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ======================= KEYBOARD ======================= */
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); /* ESC - выход */
}

/* ======================= MAIN ======================= */
int main(int argc, char** argv) {
    printf("Select program:\n");
    printf("1 - Fireworks\n");
    printf("2 - Rotating Square\n");
    printf("Your choice (1 or 2): ");
    scanf("%d", &selectedProgram);

    while (selectedProgram != 1 && selectedProgram != 2) {
        printf("Error! Please enter 1 or 2: ");
        scanf("%d", &selectedProgram);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WinWidth, WinHeight);
    glutCreateWindow(selectedProgram == 1 ? "Fireworks" : "Rotating Square");

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    srand(time(NULL));

    if (selectedProgram == 2) {
        cubePosX = rand() % (WinWidth - (int)squareSize) + squareSize / 2;
        cubePosY = rand() % (WinHeight - (int)squareSize) + squareSize / 2;
        speedX = (rand() % 5) + 2.0f;
        speedY = (rand() % 5) + 2.0f;
        if (rand() % 2) speedX = -speedX;
        if (rand() % 2) speedY = -speedY;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}