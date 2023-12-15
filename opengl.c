#include "local.h"

// Functions for openGL
void display(void);
void reshape(int w, int h);
void timer(int z);
void drawTellers();
void drawCashier();
void drawText();
void renderText(int, int, void *, char *);
void readFromSharedMemory();
void drawDataHall();
void drawMainHall();

// Parameters will cahange by reading from shared memory
int cashierID[MAXCASHIERS];
int cashier_Process = 0;
int Cashier_Behavior[MAXCASHIERS];
int num_Customers = 0;
int cashier_Queue_size[MAXCASHIERS];
int leaveCashiers = 0;
int customersLeave = 0;
double Income[MAXCASHIERS];
int rate = 1000 / 120;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Super Market");
    glLineWidth(5);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
}

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(255, 0, 255);

    // Drawing objects in the scene

    glLoadIdentity();
    drawDataHall();
    drawTellers();
    drawMainHall();
    readFromSharedMemory();
    drawCashier();
    drawText();
    glutSwapBuffers();
}

void reshape(int w, int h)
{

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-20, 20, -20, 20);
    glMatrixMode(GL_MODELVIEW);
}

// Timer function to control the animation and display FPS
void timer(int z)
{

    glutTimerFunc(rate, timer, 0);

    glutPostRedisplay();

    // readFromSharedMemory();
}

// Function for drawing supermarket background image

void drawTellers()

{

    glBegin(GL_POLYGON_BIT);

    glColor3f(0.1, 0.9, 0.4);

    glVertex2i(-20, 12);

    glVertex2i(-12, 12);

    glVertex2i(-20, 6);

    glVertex2i(-12, 6);

    glEnd();


    glBegin(GL_POLYGON_BIT);

    glColor3f(0.6, 0.4, 0.9);

    glVertex2i(-20, -6);

    glVertex2i(-12, -6);

    glVertex2i(-20, 0);

    glVertex2i(-12, 0);

    glEnd();


    glBegin(GL_POLYGON_BIT);

    glColor3f(0.3, 0.8, 0.2);

    glVertex2i(-20, 0);

    glVertex2i(-12, 0);

    glVertex2i(-20, 6);

    glVertex2i(-12, 6);

    glEnd();


    glBegin(GL_POLYGON_BIT);

    glColor3f(0.356, 0.514, 0.999);

    glVertex2i(-20, -6);

    glVertex2i(-12, -6);

    glVertex2i(-20, -12);

    glVertex2i(-12, -12);

    glEnd();


    glBegin(GL_POLYGON_BIT);

    glColor3f(1.0, 0.0, 0.0);

    glVertex2i(-19, -14);

    glVertex2i(-8, -14);

    glVertex2i(-19, -20);

    glVertex2i(-8, -20);

    glEnd();
}

void drawMainHall()

{

    glColor3f(0, 0, 255); // Blue color

    glBegin(GL_LINES);

    glVertex2f(-20.0, -13.0);

    glVertex2f(-12.0, -13.0);

    glEnd();


    glBegin(GL_LINES);

    glVertex2f(-20.0, 13.0);

    glVertex2f(-12.0, 13.0);

    glEnd();


    glBegin(GL_LINES);

    glVertex2f(-12.0, -13.0);

    glVertex2f(-12.0, 13.0);

    glEnd();

    glFlush();
}

// Function for drawing cashier

void drawCashier()

{

    glColor3f(0.8, 0.6, 0.2); // Color for the cashier

    for (int i = 0; i < cashier_Process; ++i)

    {

        double startX = -9.0 + i * 3.0; // Adjust the starting X position for each cashier

        double startY = 7.0;

        glBegin(GL_QUADS);

        glVertex2f(startX, startY);

        glVertex2f(startX + 2.0, startY);

        glVertex2f(startX + 2.0, startY + 4.0); // Adjust the height of the cashier

        glVertex2f(startX, startY + 4.0);

        glEnd();
    }
}

void drawDataHall()

{

    glColor3ub(255, 0, 255);

    glBegin(GL_LINES);

    glVertex2f(-10.0, -13.0);

    glVertex2f(-10.0, 13.0);

    glEnd();


    glBegin(GL_LINES);

    glVertex2f(-10.0, 13.0);

    glVertex2f(500.0, 13.0);

    glEnd();
    

    glBegin(GL_LINES);

    glVertex2f(-10.0, -13.0);

    glVertex2f(50.0, -13.0);

    glEnd();

    glFlush();
}

// Function for drawing text

void drawText()

{

    glColor3f(1.0, 0.8, 0.5);

    char

        mainHallNumCashier[30] = "# Cashiers : ",

        mainHallNumCustomer[30] = "# Customers : ",

        cashierId[30] = "Cashier ID ",

        cashierBehavior[30] = "Cashier Behavior ",

        numCustomer[30] = "# of Customers ",

        income[30] = "Income ",

        lCashiers[30] = "Left Cashiers : ",

        lcustomers[30] = "Left Customers : ",

        // MaxIncomeStr[30] = "Max income : ",

        cashierid[30] = "",

        numberOfcashierBehavior[5] = "",

        numberOfCustomerInQueue[5] = "",

        numberOfIncome[7] = "",

        numberOfCashierInTheMainHall[6] = "",

        numberOfCustomerInTheMainHall[6] = "",

        numOfLeftCash[3] = "",

        numOfLeftCust[3] = "";

    sprintf(numberOfCashierInTheMainHall, "%d", cashier_Process);

    sprintf(numberOfCustomerInTheMainHall, "%d", num_Customers);

    sprintf(numOfLeftCust, "%d", customersLeave);

    sprintf(numOfLeftCash, "%d", leaveCashiers);

    for (int i = 0; i < cashier_Process; ++i)

    {

        sprintf(cashierid, "%d", cashierID[i]);

        renderText(-9.0 + i * 3, 8, GLUT_BITMAP_TIMES_ROMAN_24, cashierid);

        sprintf(numberOfcashierBehavior, "%d", Cashier_Behavior[i]);

        renderText(-9.0 + i * 3, 3, GLUT_BITMAP_TIMES_ROMAN_24, numberOfcashierBehavior);

        sprintf(numberOfCustomerInQueue, "%d", cashier_Queue_size[i]);

        renderText(-9.0 + i * 3, -3, GLUT_BITMAP_TIMES_ROMAN_24, numberOfCustomerInQueue);

        sprintf(numberOfIncome, "%.1f", Income[i]);

        renderText(-9.0 + 3 * i, -9, GLUT_BITMAP_TIMES_ROMAN_24, numberOfIncome);
    }

    strcat(mainHallNumCashier, numberOfCashierInTheMainHall);

    strcat(mainHallNumCustomer, numberOfCustomerInTheMainHall);

    strcat(lCashiers, numOfLeftCash);

    strcat(lcustomers, numOfLeftCust);

    renderText(-18, 18, GLUT_BITMAP_TIMES_ROMAN_24, mainHallNumCashier);

    renderText(-18, 15, GLUT_BITMAP_TIMES_ROMAN_24, mainHallNumCustomer);

    renderText(-18, 8, GLUT_BITMAP_TIMES_ROMAN_24, cashierId);

    renderText(-19, 3, GLUT_BITMAP_TIMES_ROMAN_24, cashierBehavior);

    renderText(-19, -3, GLUT_BITMAP_TIMES_ROMAN_24, numCustomer);

    renderText(-18, -9, GLUT_BITMAP_TIMES_ROMAN_24, income);

    renderText(-18, -16, GLUT_BITMAP_TIMES_ROMAN_24, lCashiers);

    renderText(-18, -19, GLUT_BITMAP_TIMES_ROMAN_24, lcustomers);
}

// Function for render text

void renderText(int x, int y, void *font, char *string)

{

    glRasterPos2f(x, y);

    int len, i;

    len = strlen(string);

    for (i = 0; i < len && string[i] != '\0'; i++)

    {

        glutBitmapCharacter(font, string[i]);
    }
}

// Function for read data from shared memory

void readFromSharedMemory()

{

    GetNumbers(&cashier_Process, &num_Customers, &leaveCashiers, &customersLeave);

    readCashierSharedMemory(Cashier_Behavior, Income, cashier_Queue_size, cashierID);
}
