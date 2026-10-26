#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

const string ARCHIVO_HISTORIAL = "historial.txt";
const double TARIFA_BASE = 3.0; 
const double COSTO_POR_KM = 1.5;  


string entero(int valor) {
    ostringstream ss;
    ss << valor;
    return ss.str();
}

string dosDecimales(double valor) {
    ostringstream ss;
    ss << fixed << setprecision(2) << valor;
    return ss.str();
}

string soles(double valor) { return "S/ " + dosDecimales(valor); }
string km(double valor) { return dosDecimales(valor) + " km"; }

string fechaHora() {
    time_t ahora = time(0);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", localtime(&ahora));
    return buffer;
}

string leerTexto(string mensaje) {
    string texto;
    while (true) {
        cout << mensaje;
        if (!getline(cin, texto)) {   
            cout << "\nEntrada cerrada. Fin del programa.\n";
            exit(0);
        }
        if (!texto.empty()) return texto;
        cout << "No puedes dejarlo vacio.\n";
    }
}

int leerEntero(string mensaje) {
    while (true) {
        stringstream ss(leerTexto(mensaje));
        int valor;
        char sobra;
        if ((ss >> valor) && !(ss >> sobra)) return valor;
        cout << "Entrada no valida: escribe un numero entero.\n";
    }
}

double leerNumero(string mensaje) {
    while (true) {
        stringstream ss(leerTexto(mensaje));
        double valor;
        char sobra;
        if ((ss >> valor) && !(ss >> sobra)) return valor;
        cout << "Entrada no valida: escribe un numero (ejemplo 6.5).\n";
    }
}


class Ubicacion {
private:
    string direccion;
    double x;
    double y;
public:
    Ubicacion(string dir = "", double posX = 0.0, double posY = 0.0) {
        direccion = dir;
        x = posX;
        y = posY;
    }
    double calcularDistancia(Ubicacion otra) {
        return abs(x - otra.x) + abs(y - otra.y);
    }
    string getDireccion() const { return direccion; }
    double getX() const { return x; }
    double getY() const { return y; }
    void setDireccion(string dir) { direccion = dir; }
    void setX(double posX) { x = posX; }
    void setY(double posY) { y = posY; }

    string texto() const {
        return direccion + " (" + dosDecimales(x) + ", " + dosDecimales(y) + ")";
    }
};

class ItemMenu {
private:
    int id;          
    string nombre;
    double precio;

public:
    ItemMenu(int i = 0, string nom = "", double prec = 0.0) {
        id = i;
        nombre = nom;
        precio = prec;
    }

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    double getPrecio() const { return precio; }

    string texto() const {
        return entero(id) + " - " + nombre + " [" + soles(precio) + "]";
    }

    void mostrar() const {
        cout << texto();
    }
};


class Establecimiento {
private:
    int id;                
    string nombre;
    Ubicacion ubicacion;
    ItemMenu* menu;         
    int capacidadMenu;
    int totalMenu;

public:
    Establecimiento(int i = 0, string nom = "", Ubicacion ubi = Ubicacion(), int cap = 10) {
        id = i;
        nombre = nom;
        ubicacion = ubi;
        capacidadMenu = cap;
        totalMenu = 0;
        menu = new ItemMenu[capacidadMenu];
    }

    ~Establecimiento() {
        delete[] menu;
    }

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    Ubicacion getUbicacion() const { return ubicacion; }

    void agregarItemMenu(string nombreItem, double precio) {
        if (totalMenu < capacidadMenu) {
            menu[totalMenu] = ItemMenu(id * 100 + totalMenu + 1, nombreItem, precio);
            totalMenu++;
        }
    }

    string textoMenu() const {
        string texto = "=== MENU DE " + nombre + " (ID " + entero(id) + ") ===";
        for (int i = 0; i < totalMenu; i++) {
            texto += "\n" + menu[i].texto();
        }
        return texto;
    }

    void mostrarMenu() const {
        cout << "\n" << textoMenu() << endl;
    }

    ItemMenu getItem(int index) const {
        if (index >= 0 && index < totalMenu) return menu[index];
        return ItemMenu(0, "", 0.0);
    }

    int getTotalMenu() const { return totalMenu; }

    int buscarItem(int idItem) const {
        for (int i = 0; i < totalMenu; i++) {
            if (menu[i].getId() == idItem) return i;
        }
        return -1;
    }
};


class Mensajero {
private:
    string nombre;
    string vehiculo;
    Ubicacion gps;
    bool disponible;
public:
    Mensajero(string nom = "", string veh = "", Ubicacion g = Ubicacion(), bool disp = true) {
        nombre = nom;
        vehiculo = veh;
        gps = g;
        disponible = disp;
    }
    void actualizarEstado(bool nuevoEstado) {
        disponible = nuevoEstado;
    }
    void actualizarUbicacion(Ubicacion nuevaGps) {
        gps = nuevaGps;
    }
    string getNombre() const { return nombre; }
    string getVehiculo() const { return vehiculo; }
    Ubicacion getGps() const { return gps; }
    bool isDisponible() const { return disponible; }
};


class Cliente {
private:
    string nombre;
    Ubicacion ubicacion;

public:
    Cliente(string nom = "", Ubicacion ubi = Ubicacion()) {
        nombre = nom;
        ubicacion = ubi;
    }

    string getNombre() const { return nombre; }
    Ubicacion getUbicacion() const { return ubicacion; }

    void registrar() {
        cout << "\n--- REGISTRO DEL CLIENTE ---\n";
        nombre = leerTexto("Nombre: ");
        string direccion = leerTexto("Direccion o referencia: ");
        double x = leerNumero("Ubicacion GPS - coordenada X (ejemplo 6.5): ");
        double y = leerNumero("Ubicacion GPS - coordenada Y (ejemplo 8): ");
        ubicacion = Ubicacion(direccion, x, y);
    }

    string texto() const {
        return nombre + " - " + ubicacion.texto();
    }
};


class Orden {
private:
    static const int MAX_LINEAS = 20;

    int numero;                       
    Cliente cliente;
    Establecimiento* establecimiento;  
    Mensajero* mensajero;              

    ItemMenu items[MAX_LINEAS];       
    int cantidades[MAX_LINEAS];
    int totalLineas;

    double subtotal;
    double distanciaRecojo;            
    double distanciaEntrega;           
    double tarifa;
    string estado;                     

public:
    Orden(int num, Cliente cli) {
        numero = num;
        cliente = cli;
        establecimiento = NULL;
        mensajero = NULL;
        totalLineas = 0;
        subtotal = 0.0;
        distanciaRecojo = 0.0;
        distanciaEntrega = 0.0;
        tarifa = 0.0;
        estado = "Pendiente";
    }

    
    void procesar(Establecimiento* tiendas[], int totalTiendas, Mensajero mensajeros[], int totalMensajeros) {
        informar("\n========== SOLICITUD #" + entero(numero) + " ==========");
        informar("Fecha: " + fechaHora());
        informar("Cliente: " + cliente.texto());

        mostrarUbicaciones(tiendas, totalTiendas, mensajeros, totalMensajeros);

        if (elegirEstablecimiento(tiendas, totalTiendas)) {
            tomarPedido();
            if (totalLineas == 0) {
                informar("\nEl carrito esta vacio. Solicitud cancelada.");
            } else if (asignarMensajero(mensajeros, totalMensajeros)) {
                calcularRutaYTarifa();
                simularEntrega();
            }
        }

        informar("\n========== FIN DE LA SOLICITUD #" + entero(numero) + " ==========");
        cout << "(Solicitud #" << numero << " guardada en " << ARCHIVO_HISTORIAL << ")\n";
    }

    
    static int contarSolicitudes() {
        ifstream archivo(ARCHIVO_HISTORIAL.c_str());
        string linea;
        int total = 0;
        while (getline(archivo, linea)) {
            if (linea.find("========== SOLICITUD #") == 0) total++;
        }
        return total;
    }

    
    static void leerHistorial() {
        ifstream archivo(ARCHIVO_HISTORIAL.c_str());
        if (!archivo) {
            cout << "\nAun no hay solicitudes en el historial.\n";
            return;
        }
        cout << "\n################ HISTORIAL DE SOLICITUDES ################\n";
        string linea;
        while (getline(archivo, linea)) {
            cout << linea << endl;
        }
        cout << "################ FIN DEL HISTORIAL ################\n";
    }

private:
    double getTotal() const { return subtotal + tarifa; }

    
    void guardar(string linea) const {
        ofstream archivo(ARCHIVO_HISTORIAL.c_str(), ios::app);   
        if (!archivo) {
            cout << "AVISO: no se pudo escribir en " << ARCHIVO_HISTORIAL << endl;
            return;
        }
        archivo << linea << "\n";
    }

    void informar(string linea) const {
        cout << linea << endl;
        guardar(linea);
    }

    
    void mostrarUbicaciones(Establecimiento* tiendas[], int totalTiendas,
                            Mensajero mensajeros[], int totalMensajeros) const {
        informar("\n--- UBICACIONES (GPS) ---");

        informar("Establecimientos:");
        for (int i = 0; i < totalTiendas; i++) {
            informar("  [ID " + entero(tiendas[i]->getId()) + "] " + tiendas[i]->getNombre() +
                    " - " + tiendas[i]->getUbicacion().texto());
        }

        informar("Mensajeros:");
        for (int i = 0; i < totalMensajeros; i++) {
            string disponibilidad = mensajeros[i].isDisponible() ? "Disponible" : "Ocupado";
            informar("  " + mensajeros[i].getNombre() + " (" + mensajeros[i].getVehiculo() + ") - " +
                    mensajeros[i].getGps().texto() + " - " + disponibilidad);
        }

        informar("Cliente:");
        informar("  " + cliente.texto());
    }

    bool elegirEstablecimiento(Establecimiento* tiendas[], int total) {
        while (true) {
            int id = leerEntero("\nID del establecimiento (0 para cancelar): ");
            if (id == 0) {
                informar("El cliente cancelo la solicitud.");
                return false;
            }
            for (int i = 0; i < total; i++) {
                if (tiendas[i]->getId() == id) {
                    establecimiento = tiendas[i];
                    informar("Establecimiento elegido: [ID " + entero(id) + "] " + establecimiento->getNombre() +
                            " - " + establecimiento->getUbicacion().texto());
                    return true;
                }
            }
            cout << "No existe un establecimiento con ese ID.\n";
        }
    }

    
    void tomarPedido() {
        informar("\n" + establecimiento->textoMenu());

        while (true) {
            int idItem = leerEntero("\nID del producto a agregar (0 para terminar): ");
            if (idItem == 0) break;

            int pos = establecimiento->buscarItem(idItem);
            if (pos == -1) {
                cout << "Ese ID no existe en el menu de " << establecimiento->getNombre() << ".\n";
                continue;
            }

            int cantidad = leerEntero("Cantidad: ");
            if (cantidad <= 0) {
                cout << "La cantidad debe ser mayor que 0.\n";
                continue;
            }
            agregarItem(establecimiento->getItem(pos), cantidad);
        }

        if (totalLineas > 0) mostrarCarrito();
    }

    void agregarItem(ItemMenu item, int cantidad) {
        
        int linea = -1;
        for (int i = 0; i < totalLineas; i++) {
            if (items[i].getId() == item.getId()) linea = i;
        }
        if (linea == -1) {
            if (totalLineas >= MAX_LINEAS) {
                cout << "El carrito esta lleno.\n";
                return;
            }
            linea = totalLineas;
            items[linea] = item;
            cantidades[linea] = 0;
            totalLineas++;
        }
        cantidades[linea] += cantidad;
        subtotal += item.getPrecio() * cantidad;

        informar("Agregado al carrito: " + entero(cantidad) + " x " + item.getNombre() +
                " (ID " + entero(item.getId()) + ") | subtotal actual: " + soles(subtotal));
    }

    void mostrarCarrito() const {
        informar("\n--- CARRITO FINAL ---");
        for (int i = 0; i < totalLineas; i++) {
            informar("  " + entero(cantidades[i]) + " x " + items[i].getNombre() +
                     " (ID " + entero(items[i].getId()) + ") = " + soles(items[i].getPrecio() * cantidades[i]));
        }
        informar("  Subtotal: " + soles(subtotal));
    }

    
    bool asignarMensajero(Mensajero mensajeros[], int total) {
        informar("\n--- ASIGNACION DE MENSAJERO ---");
        informar("Distancia de cada mensajero a " + establecimiento->getNombre() + ":");

        Ubicacion posLocal = establecimiento->getUbicacion();
        int mejor = -1;
        double menor = 0.0;

        for (int i = 0; i < total; i++) {
            if (!mensajeros[i].isDisponible()) {
                informar("  " + mensajeros[i].getNombre() + ": ocupado (no se considera)");
                continue;
            }
            double d = mensajeros[i].getGps().calcularDistancia(posLocal);
            informar("  " + mensajeros[i].getNombre() + " (" + mensajeros[i].getVehiculo() + ") desde " +
                    mensajeros[i].getGps().texto() + ": " + km(d));
            if (mejor == -1 || d < menor) {
                mejor = i;
                menor = d;
            }
        }

        if (mejor == -1) {
            informar("No hay mensajeros disponibles. Solicitud cancelada.");
            return false;
        }

        mensajero = &mensajeros[mejor];
        mensajero->actualizarEstado(false);  
        informar("Mensajero asignado (el mas cercano): " + mensajero->getNombre() + " - " +
                mensajero->getVehiculo() + " | estado: Ocupado");
        return true;
    }

    void calcularRutaYTarifa() {
        Ubicacion posMensajero = mensajero->getGps();
        Ubicacion posLocal = establecimiento->getUbicacion();
        Ubicacion posCliente = cliente.getUbicacion();

        distanciaRecojo = posMensajero.calcularDistancia(posLocal);
        distanciaEntrega = posLocal.calcularDistancia(posCliente);
        double total = distanciaRecojo + distanciaEntrega;
        tarifa = TARIFA_BASE + COSTO_POR_KM * total;

        informar("\n--- RUTA DE " + mensajero->getNombre() + " ---");
        informar("Tramo 1 (al establecimiento): " + posMensajero.texto() + " -> " + posLocal.texto() +
                " = " + km(distanciaRecojo));
        informar("Tramo 2 (al cliente):         " + posLocal.texto() + " -> " + posCliente.texto() +
                " = " + km(distanciaEntrega));
        informar("Distancia total del recorrido: " + km(total));

        informar("\n--- TARIFA DE ENTREGA ---");
        informar("Tarifa = " + soles(TARIFA_BASE) + " (base) + " + km(total) + " x " + soles(COSTO_POR_KM) +
                " = " + soles(tarifa));
        informar("Subtotal del pedido: " + soles(subtotal));
        informar("TOTAL A PAGAR:       " + soles(getTotal()));
    }

    void simularEntrega() {
        informar("\n--- SEGUIMIENTO DE LA ORDEN ---");
        informar("Estado: " + estado + " -> " + mensajero->getNombre() + " sale hacia " +
                establecimiento->getNombre());
        while (estado != "Entregado") {
            avanzarEstado();
        }
    }

    void avanzarEstado() {
        if (estado == "Pendiente") {
            estado = "Preparando";
            mensajero->actualizarUbicacion(establecimiento->getUbicacion()); 
            informar("Estado: Preparando -> " + mensajero->getNombre() + " llego a " +
                    establecimiento->getUbicacion().texto() + " y el pedido se esta preparando");
        } else if (estado == "Preparando") {
            estado = "En camino";
            informar("Estado: En camino -> " + mensajero->getNombre() + " lleva el pedido a " +
                    cliente.getUbicacion().texto());
        } else if (estado == "En camino") {
            estado = "Entregado";
            mensajero->actualizarUbicacion(cliente.getUbicacion());            
            mensajero->actualizarEstado(true);                                
            informar("Estado: Entregado -> pedido entregado a " + cliente.getNombre());
            informar(mensajero->getNombre() + " queda Disponible en " + mensajero->getGps().texto());
        }
    }
};

int main() {
    
    Establecimiento pizzeria(1, "Pizzeria", Ubicacion("Jr. Lima 120", 2, 3), 5);
    pizzeria.agregarItemMenu("Pizza Personal", 18.00);
    pizzeria.agregarItemMenu("Pizza Familiar", 42.00);
    pizzeria.agregarItemMenu("Lasagna", 22.50);
    pizzeria.agregarItemMenu("Gaseosa", 4.00);

    Establecimiento polleria(2, "Polleria", Ubicacion("Av. Simon Bolivar 340", 8, 2), 5);
    polleria.agregarItemMenu("Cuarto de Pollo", 16.00);
    polleria.agregarItemMenu("Medio Pollo", 30.00);
    polleria.agregarItemMenu("Porcion de Papas", 8.00);
    polleria.agregarItemMenu("Chicha Morada", 5.00);

    Establecimiento cafeteria(3, "Cafeteria", Ubicacion("Jr. Deustua 215", 5, 9), 5);
    cafeteria.agregarItemMenu("Cafe Americano", 6.00);
    cafeteria.agregarItemMenu("Jugo de Papaya", 7.50);
    cafeteria.agregarItemMenu("Sandwich Mixto", 10.00);
    cafeteria.agregarItemMenu("Torta de Chocolate", 9.00);

    Establecimiento* tiendas[3] = { &pizzeria, &polleria, &cafeteria };

    
    Mensajero mensajeros[4] = {
        Mensajero("Luis", "Moto", Ubicacion("Plaza de Armas", 3, 3), true),
        Mensajero("Rosa", "Bicicleta", Ubicacion("Parque Pino", 9, 1), true),
        Mensajero("Pedro", "Moto", Ubicacion("Terminal Terrestre", 1, 7), true),
        Mensajero("Marco", "Moto", Ubicacion("Jr. Puno 80", 8, 3), false) 
    };


    int solicitudes = Orden::contarSolicitudes();   
    int opcion = -1;

    while (opcion != 0) {
        cout << "\n=============== DELIVERY PUNO ===============\n";
        cout << "1. Nueva solicitud de delivery\n";
        cout << "2. Ver historial de solicitudes\n";
        cout << "0. Cerrar el programa\n";
        opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            Cliente cliente;
            cliente.registrar();
            solicitudes++;
            Orden orden(solicitudes, cliente);
            orden.procesar(tiendas, 3, mensajeros, 4);
        } else if (opcion == 2) {
            Orden::leerHistorial();
        } else if (opcion != 0) {
            cout << "Opcion no valida.\n";
        }
    }

    cout << "\nPrograma cerrado. Historial guardado en " << ARCHIVO_HISTORIAL << endl;
    return 0;
}