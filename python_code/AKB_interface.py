import tkinter as tk
from tkinter import ttk
import serial
import threading
import time
import csv
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Настройки последовательного порта
SERIAL_PORT = 'COM5'  # Замените на ваш порт
BAUD_RATE = 9600

# Параметры графика
MAX_POINTS = 50

# Функция проверки чисел с плавающей точкой
def is_float(value):
    try:
        float(value)
        return True
    except ValueError:
        return False

# Функция обновления графиков
def update_graphs():
    for time_data, sensor_data, line, ax in zip(
            [time_points, time_points, time_points, time_points],
            [sensor_values1, sensor_values2, sensor_values3, sensor_values4],
            [line1, line2, line3, line4],
            [ax1, ax2, ax3, ax4]):
        if len(time_data) > MAX_POINTS:
            time_data.pop(0)
            sensor_data.pop(0)

        line.set_data(time_data, sensor_data)
        ax.relim()
        ax.autoscale_view()

    canvas.draw()  # Прямое обновление холста

# Функция автоматического обновления графиков
def auto_update_graphs():
    update_graphs()
    root.after(100, auto_update_graphs)

# Функция для чтения данных с Arduino
def read_from_arduino():
    try:
        with open("arduino_data.csv", "w", newline="") as csvfile:
            csvwriter = csv.writer(csvfile)
            csvwriter.writerow(["Время", "Значение 1", "Значение 2", "Значение 3", "Значение 4"])

            while True:
                try:
                    # Чтение строки из порта
                    data = ser.readline().decode('utf-8').strip()
                    print(f"Полученные данные: {data}")  # Отладочное сообщение

                    # Разделение строки на значения
                    values = data.split()

                    # Проверка на корректность формата (должно быть 4 значения с плавающей точкой)
                    if len(values) == 4 and all(is_float(v) for v in values):
                        value1, value2, value3, value4 = map(float, values)
                        current_time = time.time() - start_time

                        # Обновление данных для графиков
                        time_points.append(current_time)
                        sensor_values1.append(value1)
                        sensor_values2.append(value2)
                        sensor_values3.append(value3)
                        sensor_values4.append(value4)

                        # Запись в CSV
                        csvwriter.writerow([current_time, value1, value2, value3, value4])

                        # Обновление значений в Tkinter
                        sensor_value1.set(value1)
                        sensor_value2.set(value2)
                        sensor_value3.set(value3)
                        sensor_value4.set(value4)
                    else:
                        print(f"Неверный формат данных: {data}")  # Отладка
                except Exception as e:
                    print(f"Ошибка при обработке данных: {e}")  # Лог ошибки
                time.sleep(0.1)
    except Exception as e:
        print(f"Ошибка при открытии файла или порта: {e}")

# Функция закрытия приложения
def close_app():
    ser.close()
    root.quit()

# Создаем окно Tkinter
root = tk.Tk()
root.title("Визуализация данных с Arduino (4 величины)")
root.geometry("900x1100")
root.config(bg="#2B2D42")

# Заголовок
title_label = tk.Label(root, text="🔌 Визуализация данных с Arduino (4 величины) 🔋", 
                       font=("Helvetica", 24, "bold"), fg="#FFFFFF", bg="#1B1E23", pady=20)
title_label.pack(fill=tk.X)

# Отображение текущих значений
sensor_value1 = tk.DoubleVar()
sensor_value2 = tk.DoubleVar()
sensor_value3 = tk.DoubleVar()
sensor_value4 = tk.DoubleVar()
sensor_value1.set(0)
sensor_value2.set(0)
sensor_value3.set(0)
sensor_value4.set(0)

value_frame = tk.Frame(root, bg="#1B1E23", pady=20, padx=10)
value_frame.pack(fill=tk.X, pady=(10, 20))

# Добавление рамки
value_inner_frame = tk.Frame(value_frame, bg="#2B2D42", pady=10, padx=20)
value_inner_frame.pack(pady=10, padx=10, fill=tk.X)

for i, (var, label_text, color) in enumerate(
    zip([sensor_value1, sensor_value2, sensor_value3, sensor_value4],
        ["⚡ напряжение", "🔋 сила тока", "🌡 температура 1", "🌡 температура 2"],
        ["#8ECAE6", "#FFB703", "#E63946", "#A2D2FF"])):
    tk.Label(value_inner_frame, text=label_text, font=("Helvetica", 16, "bold"), 
             fg="#FFFFFF", bg="#2B2D42").grid(row=i, column=0, padx=20, pady=15)
    lbl = tk.Label(value_inner_frame, textvariable=var, font=("Helvetica", 28, "bold"), 
                   fg=color, bg="#2B2D42")
    lbl.grid(row=i, column=1, padx=20, pady=15)

# Настройка графиков
fig = Figure(figsize=(8, 10), dpi=100, tight_layout=True)
gs = fig.add_gridspec(4, 1, hspace=1.2)

ax1 = fig.add_subplot(gs[0, 0])
ax2 = fig.add_subplot(gs[1, 0])
ax3 = fig.add_subplot(gs[2, 0])
ax4 = fig.add_subplot(gs[3, 0])

for ax in [ax1, ax2, ax3, ax4]:
    ax.set_xlabel("Время (секунды)", fontsize=8)
    ax.set_ylabel("Значение", fontsize=10)
    ax.grid(color="#CCCCCC", linestyle="--", linewidth=0.5)

line1, = ax1.plot([], [], color='#219EBC', linewidth=2)
line2, = ax2.plot([], [], color='#FB8500', linewidth=2)
line3, = ax3.plot([], [], color='#D62828', linewidth=2)
line4, = ax4.plot([], [], color='#8D99AE', linewidth=2)

canvas = FigureCanvasTkAgg(fig, master=root)
canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

# Подготовка для чтения данных
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time_points = []
sensor_values1 = []
sensor_values2 = []
sensor_values3 = []
sensor_values4 = []
start_time = time.time()

# Поток для чтения данных
read_thread = threading.Thread(target=read_from_arduino, daemon=True)
read_thread.start()

# Запуск автообновления графиков
auto_update_graphs()

# Запуск основного цикла Tkinter
root.protocol("WM_DELETE_WINDOW", close_app)
root.mainloop()
