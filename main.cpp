#include <windows.h>
#include <string>
#include <sstream>
#include "AVL.h"

#define ID_INSERTION 1
#define ID_TRAVERSALS 2
#define ID_SEARCH 3
#define ID_DELETION 4
#define ID_EXIT 5
#define ID_INPUT 6

#define COLOR_BACKGROUND RGB(0, 1, 21)
#define COLOR_BUTTON RGB(50, 50, 50)
#define COLOR_TEXT RGB(255, 255, 255)
#define COLOR_NODE RGB(0, 255, 0)
#define COLOR_NODE_TEXT RGB(0, 0, 0)
#define COLOR_NODE_FILL RGB(255, 255, 255)

struct nodo* root = nullptr;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset);
void performTraversal(HWND hwnd, int traversalType);
void performSearch(HWND hwnd);

// Utility function to convert in-order traversal to a string
void inOrderTraversalToString(struct nodo* node, std::stringstream& ss) {
    if (node == nullptr) return;
    inOrderTraversalToString(node->izquierda, ss);
    ss << node->clave << " ";
    inOrderTraversalToString(node->derecha, ss);
}

void preOrderTraversalToString(struct nodo* node, std::stringstream& ss) {
    if (node == nullptr) return;
    ss << node->clave << " ";
    preOrderTraversalToString(node->izquierda, ss);
    preOrderTraversalToString(node->derecha, ss);
}

void postOrderTraversalToString(struct nodo* node, std::stringstream& ss) {
    if (node == nullptr) return;
    postOrderTraversalToString(node->izquierda, ss);
    postOrderTraversalToString(node->derecha, ss);
    ss << node->clave << " ";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "AVL Tree GUI Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "AVL Tree Operations",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hInput;

    switch (uMsg) {
    case WM_CREATE:
        CreateWindow("BUTTON", "Insertar", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 100, 30, hwnd, (HMENU)ID_INSERTION, NULL, NULL);
        CreateWindow("BUTTON", "Recorridos", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 120, 10, 100, 30, hwnd, (HMENU)ID_TRAVERSALS, NULL, NULL);
        CreateWindow("BUTTON", "Buscar", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 230, 10, 100, 30, hwnd, (HMENU)ID_SEARCH, NULL, NULL);
        CreateWindow("BUTTON", "Eliminar", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 340, 10, 100, 30, hwnd, (HMENU)ID_DELETION, NULL, NULL);
        CreateWindow("BUTTON", "Salir", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 450, 10, 100, 30, hwnd, (HMENU)ID_EXIT, NULL, NULL);
        hInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 50, 100, 25, hwnd, (HMENU)ID_INPUT, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_INSERTION: {
            char buffer[256];
            GetWindowText(hInput, buffer, 256);
            int value = atoi(buffer);
            if (value != 0) {
                root = insertar(root, value);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case ID_TRAVERSALS:
            performTraversal(hwnd, 1);  // 0 for in-order traversal
            break;
        case ID_SEARCH:
            performSearch(hwnd);
            break;
        case ID_DELETION: {
            char buffer[256];
            GetWindowText(hInput, buffer, 256);
            int value = atoi(buffer);
            if (value != 0) {
                root = eliminar(root, value);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case ID_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_PAINT: {
	    PAINTSTRUCT ps;
	    HDC hdc = BeginPaint(hwnd, &ps);
	
	    // Establecer el color de fondo y el modo
	    SetBkMode(hdc, OPAQUE); // Asegúrate de usar OPAQUE aquí si deseas un fondo sólido
	    SetBkColor(hdc, COLOR_BACKGROUND);
	
	    // Rellenar el fondo de la ventana
	    HBRUSH hBrush = CreateSolidBrush(COLOR_BACKGROUND);
	    FillRect(hdc, &ps.rcPaint, hBrush);
	    DeleteObject(hBrush);
	
	    // Dibuja el árbol
	    if (root != nullptr) {
	        drawTree(hdc, root, 400, 100, 200);
	    }
	
	    EndPaint(hwnd, &ps);
	    break;
	}
	

    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, COLOR_BUTTON);
        SetTextColor(hdc, COLOR_TEXT);
        return (INT_PTR)CreateSolidBrush(COLOR_BUTTON);
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, COLOR_TEXT);
        SetBkMode(hdc, COLOR_BACKGROUND);
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }

    case WM_CTLCOLORDLG: {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, COLOR_BACKGROUND); //****************************/
        return (INT_PTR)CreateSolidBrush(COLOR_BACKGROUND);
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset) {
    if (node == nullptr) return;

    // Set the node fill color and create a brush
    HBRUSH hNodeBrush = CreateSolidBrush(COLOR_NODE_FILL);
    SelectObject(hdc, hNodeBrush);

    // Draw the current node
    Ellipse(hdc, x - 35, y - 35, x + 35, y + 35);

    // Set the text color and background mode for the node text
    SetTextColor(hdc, COLOR_NODE_TEXT);
    SetBkMode(hdc, COLOR_BACKGROUND);//******************/

    // Draw the node's value
    std::string value = std::to_string(node->clave);
    TextOut(hdc, x - 20, y - 12, value.c_str(), value.length());

    // Clean up the brush
    DeleteObject(hNodeBrush);

    // Set the pen color for drawing lines
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0)); // White pen
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw lines to children
    if (node->izquierda) {
        MoveToEx(hdc, x, y + 20, NULL); // Move to the bottom of the current node
        LineTo(hdc, x - xOffset, y + 80); // Draw line to the left child
        drawTree(hdc, node->izquierda, x - xOffset, y + 100, xOffset / 2);
    }
    if (node->derecha) {
        MoveToEx(hdc, x, y + 20, NULL); // Move to the bottom of the current node
        LineTo(hdc, x + xOffset, y + 80); // Draw line to the right child
        drawTree(hdc, node->derecha, x + xOffset, y + 100, xOffset / 2);
    }

    // Restore the old pen and clean up
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}



// Función auxiliar para mostrar MessageBox con colores correctos
void showColoredMessageBox(HWND hwnd, const char* message, const char* title, UINT type) {
    // Guardar los colores actuales del sistema
    COLORREF oldBkColor = GetSysColor(COLOR_WINDOW);
    COLORREF oldTextColor = GetSysColor(COLOR_WINDOWTEXT);

    // Establecer nuevos colores (fondo negro, texto blanco)
    SetSysColors(1, (CONST INT[]){COLOR_WINDOW}, (CONST COLORREF[]){RGB(0,0,0)});
    SetSysColors(1, (CONST INT[]){COLOR_WINDOWTEXT}, (CONST COLORREF[]){RGB(255,255,255)});

    // Mostrar el MessageBox
    MessageBox(hwnd, message, title, type);

    // Restaurar los colores originales
    SetSysColors(1, (CONST INT[]){COLOR_WINDOW}, &oldBkColor);
    SetSysColors(1, (CONST INT[]){COLOR_WINDOWTEXT}, &oldTextColor);
}

void performTraversal(HWND hwnd, int traversalType) {
    if (root == nullptr) {
        showColoredMessageBox(hwnd, "El árbol está vacío. Inserta algunos nodos primero.", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    std::stringstream ss;
    switch (traversalType) {
        case 0: { 
            ss << "In-order traversal: ";
            inOrderTraversalToString(root, ss);
            break;
        }
        case 1: {
            ss << "Pre-Order traversal: ";   
            preOrderTraversalToString(root, ss);
            break;
        }
        case 2: {
            ss << "Post-Order traversal: ";    
            postOrderTraversalToString(root, ss);
            break;
        }
    }
    showColoredMessageBox(hwnd, ss.str().c_str(), "Traversal Result", MB_OK | MB_ICONINFORMATION);
}


void performSearch(HWND hwnd) {
    char buffer[256];
    GetWindowText(GetDlgItem(hwnd, ID_INPUT), buffer, 256);
    int value = atoi(buffer);
    if (value != 0) {
        ResultadoBusqueda result = dfs(root, value);
        std::stringstream ss;
        if (result.encontrado) {
            ss << "Value " << value << " found at level " << result.nivel
               << "\nPosition: " << result.posicion;
        } else {
            ss << "Value " << value << " not found in the tree.";
        }
        
        showColoredMessageBox(hwnd, ss.str().c_str(), "Search Result", MB_OK);
    }
}
