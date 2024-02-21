# **SWE4-Übungen - SS2024 - Übungszettel 1 - Ausarbeitung**

## **Beispiel 1**

### **Lösungsidee**

#### Überblick

Die Implementierung des Breakout-Spiels unter Verwendung der MiniLib-Bibliothek orientiert sich an den klassischen Spielmechaniken, wobei der Spieler einen Schläger kontrolliert, um einen Ball gegen eine Wand aus Ziegeln (Bricks) zu schlagen. Jeder getroffene Ziegel verschwindet, und der Spieler erhält Punkte. Das Spiel endet, wenn der Ball den unteren Bildschirmrand erreicht.

#### Kernkomponenten und Datenstrukturen

##### Klassenstruktur

- **Shape**: Abstrakte Basisklasse für alle Spielobjekte, die eine Achsen-ausgerichtete Bounding Box (`wxRect aabb`), einen Stift (`wxPen pen`) und einen Pinsel (`wxBrush brush`) umfasst. Enthält virtuelle Methoden für Zeichnen (`do_draw`) und Bewegung.
- **Paddle**, **Ball**, **Brick**: Abgeleitete Klassen von `Shape`, die spezifische Zeichen- und Verhaltenslogiken implementieren. `Brick` beinhaltet zusätzlich eine Punktzahl (`int points`), die beim Zerstören des Ziegels zum Spieler-Score addiert wird.
- **DrawApplication** und **Window**: Klassen für die Applikationssteuerung und das Hauptfenster. `Window` behandelt Spielereignisse wie Timer-Ticks, Tastatureingaben und Mausbewegungen.

##### Spiellogik und Algorithmen

- **Kollisionsdetektion**: Beim Ball wird geprüft, ob seine Bounding Box mit der eines Bricks oder des Paddles überlappt. Bei einer Kollision mit einem Brick wird dieser entfernt und der Score erhöht. Trifft der Ball auf das Paddle, wird seine Bewegungsrichtung abhängig vom Treffpunkt angepasst.
- **Bewegung und Geschwindigkeitsanpassung**: Der Ball bewegt sich kontinuierlich über das Spielfeld. Seine Geschwindigkeit (Delta X, Delta Y) ändert sich bei Kollisionen mit dem Spielfeldrand, dem Paddle oder den Bricks. Der Schwierigkeitsgrad erhöht sich durch eine schrittweise Geschwindigkeitssteigerung.
- **Spielende und Neustart**: Erreicht der Ball den unteren Rand des Fensters, wird eine `gameOver`-Funktion aufgerufen, die eine Nachricht anzeigt und das Spiel zurücksetzt.

##### Schlüsselfunktionen und Methoden

- `on_timer`: Steuert die Spiellogik, die bei jedem Timer-Tick ausgeführt wird, einschließlich Bewegung des Balls und Kollisionsprüfung.
- `on_paint`: Verantwortlich für das Zeichnen aller Spielobjekte auf dem Bildschirm.
- `on_key` und `on_mouse_move`: Behandeln Spielerinteraktionen zur Steuerung des Paddles.

#### Zusammenfassung

Die Implementierung baut auf einer klaren Klassenhierarchie auf, wobei `Shape` als Basisklasse für die Spielobjekte dient und von `Paddle`, `Ball` und `Brick` erweitert wird. Die Spiellogik wird hauptsächlich in der `Window`-Klasse abgewickelt, die für die Verarbeitung von Benutzereingaben, die Aktualisierung des Spielzustands und das Zeichnen des Spiels verantwortlich ist. Durch die schrittweise Erhöhung der Ballgeschwindigkeit wird eine zunehmende Herausforderung und Dynamik ins Spiel gebracht. Abschließend ermöglicht die `gameOver`-Funktion das Ende und den Neustart des Spiels.


### **Testfälle**

Zur besseren Darstellung der Funktionen habe ich ein Video auf Youtube hochgeladen.

## Video-Demonstration
Ein [Video](https://www.youtube.com/watch?v=IFiSJ-WxCOk) auf Youtube zeigt die Testfälle in Aktion:

- **0:00-0:15** Menü Konfigurationsparameter: Anzahl der Ziegel Zeilen
- **0:15-0:35** Menü Konfigurationsparameter: Ball Geschwindigkeit
- **0:35-1:05** Menü Konfigurationsparameter: Größe des Schlägers
- **1:05-1:30** Austrittswinkel des Balls abhängig vom Eintrittswinkel und der Position am Schläger
- **1:30-1:50** Bedienung des Schlägers auch mit der Tastatur möglich
- **1:50-3:45** Leerlauf
- **3:45-3:53** Spielende und neues Spiel durch alle Ziegel getroffen
- **3:53-4:05** Spielende und neues Spiel durch Ballverlust
