#include <glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

// создаем массив частиц
Particle particles[MAX_PARTICLES];
int numParticles = 0; // счетчик частиц

int winWidth = 1280;
int winHeight = 720;

float spawnTimer = 0.0f; // время прошедшее с появления большой частицы

// функция удаления частиц
void removeParticle(int index) {
    particles[index] = particles[numParticles - 1];
    numParticles--;
}

// функция создания большой частицы
void spawnShell(float x, float y) {
    if (numParticles >= MAX_PARTICLES) 
        return;
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

// создание маленьких частиц
void spawnFragment(float x, float y, float vx, float vy) {
    if (numParticles >= MAX_PARTICLES)
        return;
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

// функция взрыва большого шара
void explodeShell(int index) { 
    Particle* p = &particles[index];
    int count = 100 + rand() % 20; // количество вылетающих частиц
    for (int i = 0; i < count; ++i) { 
        float angle = (float)i / count * 2.0f * PI; // угол вылета
        float speed = 150.0f + rand() % 80;
        spawnFragment(
            p->x,
            p->y,
            cosf(angle) * speed, 
            sinf(angle) * speed 
        );
    }
    removeParticle(index); // удаляем большую частицу
}

// обработка физики
void stepSimulation(float dt) { // dt - время одного шага
    for (int i = 0; i < numParticles; ) { 
        Particle* p = &particles[i];
        if (p->alive == 0) { // если частица мертва, то пропускаем её
            i++;
            continue;
        }
        p->vy -= GRAVITY * dt; // воздействие гравитации на скорость частицы
        p->x += p->vx * dt;
        p->y += p->vy * dt;
        p->life -= dt;
        if (p->type == PARTICLE_SHELL) {
            p->radius += 8.0f * dt; // увеличиваем радиус с течением времени
            if (p->radius >= MAX_RADIUS || p->life <= 0.0f || p->vy <= 0) {
                explodeShell(i);
                continue;
            }
        }
        else {
            if (p->life <= 0.0f) {
                removeParticle(i);
                continue;
            }
        }
        i++;
    }
}

// функция отрисовки круга
void drawCircle(int x, int y, int r) {
    glBegin(GL_POLYGON);        
    for (int i = 0; i < 50; i++) {
        float angle = 2.0 * PI * (float)i / 50.0;
        float dx = r * cosf(angle);
        float dy = r * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// функция отрисовки частиц
void drawScene() { 
    for (int i = 0; i < numParticles; ++i) {
        Particle* p = &particles[i];
        if (!p->alive) // про пускаем все мертвые частицы
            continue;
        if (p->type == PARTICLE_SHELL) // если частица большая, то рисуем одним цветом
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(1.0f, 0.4f, 0.2f); // а если маленькая, то другим
        drawCircle(p->x, p->y, p->radius);
    }
}

// функция полной отрисовки 
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawScene();
    glutSwapBuffers();
}

// функция времени для плавной анимации
void timer(int value) {
    const float dt = 1.0f / 60.0f;
    spawnTimer += dt;
    if (spawnTimer >= 1.0f) {
        float x = 50.0f + rand() % (winWidth - 100);
        spawnShell(x, 30.0f);
        spawnTimer = 0.0f;
    }
    stepSimulation(dt);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// функция отработки изменения размеров окна
void reshape(GLint w, GLint h) {
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Fireworks");
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
}