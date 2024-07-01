import tkinter as tk
import math
import random

GRID_SIZE = 16
CELL_SIZE = 40
WINDOW_SIZE = GRID_SIZE * CELL_SIZE
CANNON_X = GRID_SIZE // 2
CANNON_Y = GRID_SIZE - 1
BALL_SIZE = 10

class ArtilleryGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Artillery Game")
        
        self.canvas = tk.Canvas(root, width=WINDOW_SIZE, height=WINDOW_SIZE, bg="white")
        self.canvas.grid(row=1, column=0, columnspan=4)
        
        self.angle_label = tk.Label(root, text="Angle (0-180):")
        self.angle_label.grid(row=0, column=0)
        self.angle_entry = tk.Entry(root)
        self.angle_entry.grid(row=0, column=1)
        self.angle_button = tk.Button(root, text="Rotate", command=self.rotate_cannon)
        self.angle_button.grid(row=0, column=2)
        
        self.charge_label = tk.Label(root, text="Charge:")
        self.charge_label.grid(row=0, column=3)
        self.charge_entry = tk.Entry(root)
        self.charge_entry.grid(row=0, column=4)
        self.charge_button = tk.Button(root, text="Fire", command=self.fire_cannon)
        self.charge_button.grid(row=0, column=5)
        
        self.cannon_angle = 90
        self.cannon = None
        self.ball = None
        self.target = None
        self.target_x = 0
        self.target_y = 0
        
        self.initialize_game()
        
    def initialize_game(self):
        self.draw_grid()
        self.place_cannon()
        self.place_target()
        
    def draw_grid(self):
        for i in range(GRID_SIZE + 1):
            self.canvas.create_line(i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_SIZE)
            self.canvas.create_line(0, i * CELL_SIZE, WINDOW_SIZE, i * CELL_SIZE)
            
    def place_cannon(self):
        if self.cannon:
            self.canvas.delete(self.cannon)
        angle_rad = math.radians(self.cannon_angle)
        x1 = CANNON_X * CELL_SIZE + CELL_SIZE // 2
        y1 = CANNON_Y * CELL_SIZE + CELL_SIZE // 2
        x2 = x1 + 20 * math.cos(angle_rad)
        y2 = y1 - 20 * math.sin(angle_rad)
        self.cannon = self.canvas.create_line(x1, y1, x2, y2, width=5, fill="black")
        
    def place_target(self):
        if self.target:
            self.canvas.delete(self.target)
        self.target_x = random.randint(0, GRID_SIZE - 1)
        self.target_y = 0
        x1 = self.target_x * CELL_SIZE
        y1 = self.target_y * CELL_SIZE
        x2 = x1 + CELL_SIZE
        y2 = y1 + CELL_SIZE
        self.target = self.canvas.create_rectangle(x1, y1, x2, y2, fill="red")
        
    def rotate_cannon(self):
        try:
            angle = int(self.angle_entry.get())
            if 0 <= angle <= 180:
                self.cannon_angle = angle
                self.place_cannon()
        except ValueError:
            pass
        
    def fire_cannon(self):
        try:
            charge = int(self.charge_entry.get())
            if charge > 0:
                self.animate_ball(charge)
        except ValueError:
            pass
        
    def animate_ball(self, charge):
        if self.ball:
            self.canvas.delete(self.ball)
        angle_rad = math.radians(self.cannon_angle)
        x = CANNON_X
        y = CANNON_Y
        dx = math.cos(angle_rad)
        dy = -math.sin(angle_rad)
        distance = 0
        while distance < charge and 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE:
            x += dx
            y += dy
            distance += 1
            self.ball = self.canvas.create_oval(x * CELL_SIZE, y * CELL_SIZE, 
                                                x * CELL_SIZE + BALL_SIZE, 
                                                y * CELL_SIZE + BALL_SIZE, fill="blue")
            self.root.update()
            self.root.after(100)
        
        while y < GRID_SIZE - 1:
            y += 1
            self.ball = self.canvas.create_oval(x * CELL_SIZE, y * CELL_SIZE, 
                                                x * CELL_SIZE + BALL_SIZE, 
                                                y * CELL_SIZE + BALL_SIZE, fill="blue")
            self.root.update()
            self.root.after(100)
        
        if int(x) == self.target_x and int(y) == GRID_SIZE - 1:
            self.canvas.delete(self.target)
            self.place_target()
        
        self.root.after(1000, self.fire_cannon)
        
if __name__ == "__main__":
    root = tk.Tk()
    game = ArtilleryGame(root)
    root.mainloop()

