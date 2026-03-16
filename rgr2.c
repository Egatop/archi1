#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

/* Глобальные переменные */
GLint Width = 512, Height = 512;      /* Размеры окна */
float squareSize = 50.0f;             /* Размер квадрата */
float posX = 256.0f, posY = 256.0f;   /* Позиция центра квадрата */
float speedX = 3.0f, speedY = 2.5f;   /* Скорость перемещения */
float angle = 0.0f;                   /* Угол вращения */
float rotationSpeed = 1.0f;           /* Скорость вращения */
float rotationDecay = 0.99f;          /* Замедление вращения */
float maxRotationSpeed = 10.0f;       /* Максимальная скорость вращения */
float bounceRotationBoost = 2.0f;     /* Увеличение скорости вращения при отскоке */
int delay = 16;                        /* Задержка в МС (≈60 FPS) */

/* Функция для отрисовки квадрата с вращением */
void Display(void) {
    float halfSize = squareSize / 2.0f;
    
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Сохраняем текущую матрицу */
    glPushMatrix();

    /* Перемещаем квадрат в его позицию */
    glTranslatef(posX, posY, 0.0f);

    /* Применяем вращение вокруг центра квадрата */
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    /* Рисуем квадрат */
    glColor3ub(255, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-halfSize, -halfSize);
    glVertex2f(-halfSize, halfSize);
    glVertex2f(halfSize, halfSize);
    glVertex2f(halfSize, -halfSize);
    glEnd();

    /* Возвращаем матрицу в исходное состояние */
    glPopMatrix();

    glFinish();
    glutSwapBuffers();
}

void Reshape(GLint w, GLint h) {
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y) {
#define ESCAPE '\033'
    if (key == ESCAPE)
        exit(0);
}

void Idle() {
    static long lastTime = 0;
    long currentTime = clock();
    float halfSize = squareSize / 2.0f;
    int bounced = 0;  /* Используем int вместо bool (0 = false, 1 = true) */

    /* Проверяем, прошло ли достаточно времени */
    if (currentTime - lastTime < delay)
        return;

    lastTime = currentTime;

    /* Обновляем позицию */
    posX += speedX;
    posY += speedY;

    /* Проверяем столкновения с границами экрана */
    /* Столкновение с правой или левой границей */
    if (posX + halfSize > Width || posX - halfSize < 0) {
        speedX = -speedX; /* Меняем направление по X */
        bounced = 1;

        /* Корректируем позицию, чтобы квадрат не застревал за границей */
        if (posX + halfSize > Width) posX = Width - halfSize;
        if (posX - halfSize < 0) posX = halfSize;
    }

    /* Столкновение с верхней или нижней границей */
    if (posY + halfSize > Height || posY - halfSize < 0) {
        speedY = -speedY; /* Меняем направление по Y */
        bounced = 1;

        /* Корректируем позицию */
        if (posY + halfSize > Height) posY = Height - halfSize;
        if (posY - halfSize < 0) posY = halfSize;
    }

    /* Увеличиваем скорость вращения при отскоке */
    if (bounced) {
        rotationSpeed += bounceRotationBoost;

        /* Ограничиваем максимальную скорость вращения */
        if (rotationSpeed > maxRotationSpeed) {
            rotationSpeed = maxRotationSpeed;
        }
    }

    /* Уменьшаем скорость вращения со временем */
    rotationSpeed *= rotationDecay;

    /* Гарантируем минимальную скорость вращения */
    if (rotationSpeed < 0.5f) {
        rotationSpeed = 0.5f;
    }

    /* Обновляем угол вращения */
    angle += rotationSpeed;

    /* Нормализуем угол  */
    if (angle > 360.0f) {
        angle -= 360.0f;
    }

    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    /* Устанавливаем начальную позицию случайно */
    srand(time(NULL));
    posX = (float)(rand() % (Width - (int)squareSize)) + squareSize / 2.0f;
    posY = (float)(rand() % (Height - (int)squareSize)) + squareSize / 2.0f;

    /* Устанавливаем начальную скорость случайно */
    speedX = (float)(rand() % 5) + 2.0f;
    speedY = (float)(rand() % 5) + 2.0f;
    if (rand() % 2) speedX = -speedX;
    if (rand() % 2) speedY = -speedY;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Вращающийся квадрат");

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutIdleFunc(Idle);

    glutMainLoop();
    return 0;
}