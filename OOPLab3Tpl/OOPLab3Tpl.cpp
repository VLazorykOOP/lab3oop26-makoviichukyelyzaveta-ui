


#include <iostream>
#include <cmath>
#include <string>  
#include <new>
using namespace std;
//  Завдання 1: Квадрат
class Square {
private:
    double side;
    string color;

public:
    Square() : side(1), color("white") {}

    Square(double s, string c) {
        side = (s > 0) ? s : 1;
        color = c;
    }

    void setSide(double s) { if (s > 0) side = s; }
    void setColor(string c) { color = c; }

    double area() { return side * side; }
    double perimeter() { return 4 * side; }
    double inscribedCircle() { return side / 2; }
    double circumscribedCircle() { return (side * sqrt(2)) / 2; }

    void print() {
        cout << "\n--- SQUARE ---\n";
        cout << "Side:      " << side << "\n";
        cout << "Color:     " << color << "\n";
        cout << "Area:      " << area() << "\n";
        cout << "Perimeter: " << perimeter() << "\n";
    }
};
//  Завдання 2: Вектор
class Vector {
private:
    unsigned int* data;
    size_t        size;
    int           state;          // 0-OK 1-no memory 2-out of bounds 3-neg init
    static int    objectCount;

public:
    // конструктори 
    Vector() : data(nullptr), size(1), state(0) {
        data = new (nothrow) unsigned int[1]();
        if (!data) state = 1;
        objectCount++;
    }

    explicit Vector(size_t n) : data(nullptr), size(n ? n : 1), state(0) {
        data = new (nothrow) unsigned int[size]();
        if (!data) state = 1;
        objectCount++;
    }

    Vector(size_t n, int value) : data(nullptr), size(n ? n : 1), state(0) {
        if (value < 0) { state = 3; value = 0; }
        data = new (nothrow) unsigned int[size];
        if (!data) { state = 1; }
        else { for (size_t i = 0; i < size; i++) data[i] = (unsigned int)value; }
        objectCount++;
    }

    Vector(const Vector& other) : data(nullptr), size(other.size), state(other.state) {
        data = new (nothrow) unsigned int[size];
        if (!data) { state = 1; }
        else { for (size_t i = 0; i < size; i++) data[i] = other.data[i]; }
        objectCount++;
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) return *this;
        delete[] data;
        size = other.size;
        state = other.state;
        data = new (nothrow) unsigned int[size];
        if (!data) { state = 1; }
        else { for (size_t i = 0; i < size; i++) data[i] = other.data[i]; }
        return *this;
    }

    ~Vector() { delete[] data; objectCount--; }

    // доступ до елементів 

    // get (тільки читання)
    unsigned int get(size_t index) const {
        if (index >= size) return 0;
        return data[index];
    }

    // set з параметром за замовчуванням
    void set(size_t index, unsigned int value = 0) {
        if (index >= size) { state = 2; return; }
        data[index] = value;
    }

    // operator[] — повертає референс, потрібен для Matrix v[i][j]
    unsigned int& operator[](size_t index) {
        if (index >= size) { state = 2; return data[0]; } // захист
        return data[index];
    }

    unsigned int operator[](size_t index) const {
        if (index >= size) return 0;
        return data[index];
    }

    // арифметика
    Vector operator+(const Vector& other) const {
        Vector result(size);
        if (size != other.size) { result.state = 2; return result; }
        for (size_t i = 0; i < size; i++)
            result.data[i] = data[i] + other.data[i];
        return result;
    }

    Vector operator-(const Vector& other) const {
        Vector result(size);
        if (size != other.size) { result.state = 2; return result; }
        for (size_t i = 0; i < size; i++)
            result.data[i] = (data[i] >= other.data[i]) ? data[i] - other.data[i] : 0;
        return result;
    }

    // множення на unsigned short (за умовою завдання)
    Vector operator*(unsigned short value) const {
        Vector result(size);
        for (size_t i = 0; i < size; i++)
            result.data[i] = data[i] * value;
        return result;
    }

    // ділення (потрібне для Matrix::div)
    Vector operator/(unsigned int value) const {
        Vector result(size);
        if (value == 0) { result.state = 2; return result; }
        for (size_t i = 0; i < size; i++)
            result.data[i] = data[i] / value;
        return result;
    }

    // порівняння
    bool operator==(const Vector& other) const {
        if (size != other.size) return false;
        for (size_t i = 0; i < size; i++)
            if (data[i] != other.data[i]) return false;
        return true;
    }
    bool operator!=(const Vector& other) const { return !(*this == other); }
    bool operator<(const Vector& other)  const {
        size_t mn = (size < other.size) ? size : other.size;
        for (size_t i = 0; i < mn; i++) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
        }
        return size < other.size;
    }
    bool operator>(const Vector& other) const { return other < *this; }

    //друк та стан 
    void print() const {
        for (size_t i = 0; i < size; i++) cout << data[i] << " ";
        cout << "\n";
    }

    int    getState()      const { return state; }
    size_t getSize()       const { return size; }
    static int getObjectCount() { return objectCount; }
};

int Vector::objectCount = 0;
//  Завдання 3: Матриця
class Matrix {
private:
    Vector* v;
    int     rows;
    int     cols;

public:
    // 5×5
    Matrix() : rows(5), cols(5) {
        v = new Vector[rows];
        for (int i = 0; i < rows; i++) v[i] = Vector(cols);
    }

    // n×n
    explicit Matrix(int n) : rows(n), cols(n) {
        v = new Vector[rows];
        for (int i = 0; i < rows; i++) v[i] = Vector(cols);
    }

    // n×m з початковим значенням
    Matrix(int n, int m, unsigned int value) : rows(n), cols(m) {
        v = new Vector[rows];
        for (int i = 0; i < rows; i++) v[i] = Vector(cols, (int)value);
    }

    Matrix(const Matrix& m) : rows(m.rows), cols(m.cols) {
        v = new Vector[rows];
        for (int i = 0; i < rows; i++) v[i] = m.v[i];
    }

    Matrix& operator=(const Matrix& m) {
        if (this == &m) return *this;
        delete[] v;
        rows = m.rows; cols = m.cols;
        v = new Vector[rows];
        for (int i = 0; i < rows; i++) v[i] = m.v[i];
        return *this;
    }

    ~Matrix() { delete[] v; }

    // друк 
    void print() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                cout << v[i][j] << " ";    // operator[] Vector
            cout << "\n";
        }
    }

    //  додавання 
    Matrix add(const Matrix& m) const {
        if (rows != m.rows || cols != m.cols) {
            cout << "Size mismatch!\n";
            return Matrix();
        }
        Matrix res(rows, cols, 0);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                res.v[i][j] = v[i][j] + m.v[i][j];  // читання через const [], запис через ref []
        return res;
    }

    //множення на скаляр
    Matrix mul(unsigned int k) const {
        Matrix res(rows, cols, 0);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                res.v[i][j] = v[i][j] * (unsigned short)k;  // Vector::operator*(unsigned short)
        return res;
    }

    //встановити елемент 
    void setElement(int row, int col, unsigned int val) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return;
        v[row][col] = val;
    }

    //ділення на скаляр 
    Matrix div(unsigned int k) const {
        Matrix res(rows, cols, 0);
        if (k == 0) { cout << "Division by zero!\n"; return res; }
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                res.v[i][j] = v[i][j] / k;   // Vector::operator/(unsigned int)
        return res;
    }
};
//  Меню
void task1() {
    double side;
    string color;

    cout << "\n--- SQUARE ---\n";
    cout << "Enter side: ";
    cin >> side;
    cout << "Enter color: ";
    cin >> color;

    Square s(side, color);
    s.print();
}

void task2() {
    size_t n;
    cout << "\n--- VECTOR ---\n";
    cout << "Enter vector size: ";
    cin >> n;

    // Введення v1
    Vector v1(n);
    cout << "Enter " << n << " elements for v1: ";
    for (size_t i = 0; i < n; i++) {
        unsigned int val;
        cin >> val;
        v1.set(i, val);
    }

    // Введення v2
    Vector v2(n);
    cout << "Enter " << n << " elements for v2: ";
    for (size_t i = 0; i < n; i++) {
        unsigned int val;
        cin >> val;
        v2.set(i, val);
    }

    cout << "\nv1: "; v1.print();
    cout << "v2: "; v2.print();

    cout << "\nv1 + v2:\n";
    Vector v3 = v1 + v2;
    v3.print();

    cout << "\nv1 - v2:\n";
    Vector v4 = v1 - v2;
    v4.print();

    unsigned short factor;
    cout << "\nEnter multiplier for v1: ";
    cin >> factor;
    cout << "v1 * " << factor << ":\n";
    Vector v5 = v1 * factor;
    v5.print();

    cout << "\nv1 == v2: " << (v1 == v2 ? "true" : "false") << "\n";
    cout << "v1 != v2: " << (v1 != v2 ? "true" : "false") << "\n";
    cout << "v1 >  v2: " << (v1 > v2 ? "true" : "false") << "\n";
    cout << "v1 <  v2: " << (v1 < v2 ? "true" : "false") << "\n";

    cout << "\nTotal Vector objects: " << Vector::getObjectCount() << "\n";
}

void task3() {
    int rows, cols;
    cout << "\n--- MATRIX ---\n";
    cout << "Enter rows and cols for A: ";
    cin >> rows >> cols;

    // Введення матриці A
    Matrix A(rows, cols, 0);
    cout << "Enter " << rows * cols << " elements for A (row by row):\n";
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            unsigned int val;
            cin >> val;
            A.setElement(i, j, val);
        }

    // Введення матриці B (той самий розмір)
    Matrix B(rows, cols, 0);
    cout << "Enter " << rows * cols << " elements for B (row by row):\n";
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            unsigned int val;
            cin >> val;
            B.setElement(i, j, val);
        }

    cout << "\nA:\n"; A.print();
    cout << "\nB:\n"; B.print();

    cout << "\nA + B:\n";
    Matrix C = A.add(B);
    C.print();

    unsigned int k;
    cout << "\nEnter multiplier for A * k: ";
    cin >> k;
    cout << "A * " << k << ":\n";
    Matrix D = A.mul(k);
    D.print();

    cout << "\nEnter divisor for A / k: ";
    cin >> k;
    cout << "A / " << k << ":\n";
    Matrix E = A.div(k);
    E.print();
}
int main()
{
    int choice;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1 - Square\n";
        cout << "2 - Vector\n";
        cout << "3 - Matrix\n";
        cout << "0 - Exit\n";
        cout << "Choose: ";
        cin >> choice;

        switch (choice) {
        case 1: task1(); break;
        case 2: task2(); break;
        case 3: task3(); break;
        case 0: cout << "Exit\n"; break;
        default: cout << "Wrong choice\n";
        }
    } while (choice != 0);

    return 0;
   
}

