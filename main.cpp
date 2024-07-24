#include <windows.h>
#include <string>
#include <sstream>
#include "AVL.h"

#define ID_INSERTION 1
#define ID_TRAVERSALS 2
//#define ID_SEARCH 3
#define ID_DELETION 4
#define ID_EXIT 5
#define ID_INPUT 6
#define ID_DFS 7
#define ID_BFS 8

#define COLOR_BACKGROUND RGB(1, 10, 120)
#define COLOR_BUTTON RGB(50, 50, 50)
#define COLOR_TEXT RGB(255, 255, 255)
#define COLOR_NODE RGB(0, 255, 0)
#define COLOR_NODE_TEXT RGB(0, 0, 0)
#define COLOR_NODE_FILL RGB(255, 255, 255)

struct nodo* root = nullptr;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset, int level, int maxLevel);
void performTraversal(HWND hwnd, int traversalType);
void performSearch(HWND hwnd, bool isDFS);

// Utility function to convert in-order traversal to a string
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

int getTreeHeight(struct nodo* node) {
    if (node == nullptr) return 0;
    int leftHeight = getTreeHeight(node->izquierda);
    int rightHeight = getTreeHeight(node->derecha);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
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
		        CreateWindow("BUTTON", "DFS", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 230, 10, 100, 30, hwnd, (HMENU)ID_DFS, NULL, NULL);
		        CreateWindow("BUTTON", "BFS", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 340, 10, 100, 30, hwnd, (HMENU)ID_BFS, NULL, NULL);
		        CreateWindow("BUTTON", "Eliminar", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 450, 10, 100, 30, hwnd, (HMENU)ID_DELETION, NULL, NULL);
		        CreateWindow("BUTTON", "Salir", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 560, 10, 100, 30, hwnd, (HMENU)ID_EXIT, NULL, NULL);
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
        case ID_DFS:
		            performSearch(hwnd, true);  // true for DFS
		            break;
		        case ID_BFS:
		            performSearch(hwnd, false);  // false for BFS
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
	
	        // Obtener el tamaño actual de la ventana
	        RECT clientRect;
	        GetClientRect(hwnd, &clientRect);
	        int windowWidth = clientRect.right - clientRect.left;
	        int windowHeight = clientRect.bottom - clientRect.top;
	
	        // Establecer el color de fondo y el modo
	        SetBkMode(hdc, TRANSPARENT);
	        HBRUSH hBrush = CreateSolidBrush(COLOR_BACKGROUND);
	        FillRect(hdc, &clientRect, hBrush);
	        DeleteObject(hBrush);
	
	        // Dibujar el árbol
	        if (root != nullptr) {
	            int treeHeight = getTreeHeight(root);
	            int xOffset = windowWidth / 3; // Ajustar según sea necesario
	            drawTree(hdc, root, windowWidth / 2, 100, xOffset, 0, treeHeight - 1);
	        }
	
	        EndPaint(hwnd, &ps);
	        break;
	    }
	    
	case WM_SIZE:
	        // Forzar un repintado cuando se redimensiona la ventana
	        InvalidateRect(hwnd, NULL, TRUE);
	        break;    

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

// Función para dibujar el árbol
void drawTree(HDC hdc, struct nodo* node, int x, int y, int xOffset, int level, int maxLevel) {
    if (node == nullptr) return;

    // Ajustar el tamaño del nodo y la fuente según el tamaño de la ventana
    int nodeSize = 30 + (maxLevel - level) * 2; // Nodos más grandes en niveles superiores
    HFONT hFont = CreateFont(25 + (maxLevel - level), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                             DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Configurar el pincel y la pluma
    HBRUSH hNodeBrush = CreateSolidBrush(COLOR_NODE_FILL);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, hNodeBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Dibujar el nodo actual
    Ellipse(hdc, x - nodeSize, y - nodeSize, x + nodeSize, y + nodeSize);

    // Configurar y dibujar el texto del nodo
    SetTextColor(hdc, COLOR_NODE_TEXT);
    SetBkMode(hdc, TRANSPARENT);
    std::string value = std::to_string(node->clave);
    RECT textRect = {x - nodeSize, y - nodeSize, x + nodeSize, y + nodeSize};
    DrawText(hdc, value.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Calcular las posiciones de los hijos
    int nextXOffset = xOffset / 2;
    int leftX = x - nextXOffset;
    int rightX = x + nextXOffset;
    int childY = y + 80 + (maxLevel - level) * 10; // Más espacio vertical en niveles superiores

    // Dibujar líneas a los hijos y dibujar los hijos recursivamente
    if (node->izquierda) {
        MoveToEx(hdc, x, y + nodeSize, NULL);
        LineTo(hdc, leftX, childY - nodeSize);
        drawTree(hdc, node->izquierda, leftX, childY, nextXOffset, level + 1, maxLevel);
    }
    if (node->derecha) {
        MoveToEx(hdc, x, y + nodeSize, NULL);
        LineTo(hdc, rightX, childY - nodeSize);
        drawTree(hdc, node->derecha, rightX, childY, nextXOffset, level + 1, maxLevel);
    }

    // Limpiar
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hNodeBrush);
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



void performSearch(HWND hwnd, bool isDFS) {
    char buffer[256];
    GetWindowText(GetDlgItem(hwnd, ID_INPUT), buffer, 256);
    int value = atoi(buffer);
    if (value != 0) {
        ResultadoBusqueda result;
        std::string searchType;
        if (isDFS) {
            result = dfs(root, value);
            searchType = "DFS";
        } else {
            result = bfs(root, value);
            searchType = "BFS";
        }

        std::stringstream ss;
        ss << searchType << " Search Result:\n\n";
        if (result.encontrado) {
            ss << "Value " << value << " found at level " << result.nivel
               << "\nPosition: " << result.posicion << "\n\n";
        } else {
            ss << "Value " << value << " not found in the tree.\n\n";
        }
        
        // Añadir la lista de nodos visitados al mensaje
        ss << "Nodes visited in order:\n";
        Nodo<int>* nodoActual = result.nodosVisitados.getCabeza();
        while (nodoActual != nullptr) {
            ss << nodoActual->dato;
            if (nodoActual->siguiente != nullptr) {
                ss << " -> ";
            }
            nodoActual = nodoActual->siguiente;
        }
        
        showColoredMessageBox(hwnd, ss.str().c_str(), "Search Result", MB_OK | MB_ICONINFORMATION);
    }
}
