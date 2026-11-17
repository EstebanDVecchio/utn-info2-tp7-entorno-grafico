// TP7

import processing.serial.*;

Serial puerto;
boolean conectado = false;

boolean e1 = false;
boolean e2 = false;
boolean l1 = false;
boolean l2 = false;

final float FILA_ENTRADAS_Y = 100;
final float FILA_SALIDAS_Y = 200;
final float COL_ETIQUETA_X = 115;
final float COL_1_X = 285;
final float COL_2_X = 415;

final float E_ANCHO = 84;
final float E_ALTO = 48;
final float L_DIAMETRO = 64;

void setup() {
  size(520, 300);
  surface.setTitle("TP7 - Entorno Gráfico");
  textAlign(CENTER, CENTER);
  conectarSerial();
}

void draw() {
  background(255);

  // Recuadro principal
  stroke(120);
  strokeWeight(1.5);
  fill(255);
  rectMode(CORNER);
  rect(25, 25, width - 50, height - 50);

  // Etiquetas
  dibujarEtiqueta("Entradas", COL_ETIQUETA_X, FILA_ENTRADAS_Y, color(126, 166, 204));
  dibujarEtiqueta("Salidas", COL_ETIQUETA_X, FILA_SALIDAS_Y, color(245, 224, 206));

  // Entradas (E1, E2)
  dibujarEntrada("E1", COL_1_X, FILA_ENTRADAS_Y, e1);
  dibujarEntrada("E2", COL_2_X, FILA_ENTRADAS_Y, e2);

  // Salidas (L1, L2)
  boolean hoverL1 = sobreCirculo(COL_1_X, FILA_SALIDAS_Y, L_DIAMETRO / 2.0);
  boolean hoverL2 = sobreCirculo(COL_2_X, FILA_SALIDAS_Y, L_DIAMETRO / 2.0);
  dibujarSalida("L1", COL_1_X, FILA_SALIDAS_Y, l1, color(46, 204, 113));
  dibujarSalida("L2", COL_2_X, FILA_SALIDAS_Y, l2, color(231, 76, 60));

  cursor((hoverL1 || hoverL2) ? HAND : ARROW);
}

void dibujarEtiqueta(String texto, float cx, float cy, color fondo) {
  rectMode(CENTER);
  stroke(40);
  strokeWeight(1.5);
  fill(fondo);
  rect(cx, cy, 120, 44);

  fill(20);
  textSize(18);
  text(texto, cx, cy);
}

void dibujarEntrada(String nombre, float cx, float cy, boolean alto) {
  rectMode(CENTER);
  stroke(20);
  strokeWeight(2.5);
  fill(alto ? color(46, 204, 113) : color(235));
  rect(cx, cy, E_ANCHO, E_ALTO);

  fill(20);
  textSize(18);
  text(nombre, cx, cy);
}

void dibujarSalida(String nombre, float cx, float cy, boolean encendida, color colorActivo) {
  stroke(20);
  strokeWeight(2.5);
  fill(encendida ? colorActivo : color(235));
  ellipse(cx, cy, L_DIAMETRO, L_DIAMETRO);

  fill(20);
  textSize(18);
  text(nombre, cx, cy);
}

void mousePressed() {
  if (sobreCirculo(COL_1_X, FILA_SALIDAS_Y, L_DIAMETRO / 2.0)) {
    l1 = !l1;
    enviarComando(l1 ? 'A' : 'a');
  } else if (sobreCirculo(COL_2_X, FILA_SALIDAS_Y, L_DIAMETRO / 2.0)) {
    l2 = !l2;
    enviarComando(l2 ? 'B' : 'b');
  }
}

void keyPressed() {
  if (key == 'r' || key == 'R') {
    conectarSerial();
  }
}

boolean sobreCirculo(float cx, float cy, float radio) {
  return dist(mouseX, mouseY, cx, cy) <= radio;
}

void enviarComando(char cmd) {
  if (conectado && puerto != null) {
    puerto.write(cmd);
  }
}

void conectarSerial() {
  if (puerto != null) {
    puerto.stop();
    puerto = null;
  }
  conectado = false;

  String[] puertos = Serial.list();
  if (puertos.length == 0) return;

  String elegido = puertos[0];
  for (String p : puertos) {
    if (p.contains("ttyACM") || p.contains("ttyUSB") || p.startsWith("COM")) {
      elegido = p;
      break;
    }
  }

  try {
    puerto = new Serial(this, elegido, 9600);
    puerto.bufferUntil('\n');
    conectado = true;
  } catch (Exception e) {
    conectado = false;
  }
}

void serialEvent(Serial p) {
  String linea = p.readStringUntil('\n');
  if (linea == null) return;

  linea = trim(linea);
  String[] partes = split(linea, ',');
  if (partes.length == 4) {
    e1 = partes[0].equals("1");
    e2 = partes[1].equals("1");
    l1 = partes[2].equals("1");
    l2 = partes[3].equals("1");
  }
}
