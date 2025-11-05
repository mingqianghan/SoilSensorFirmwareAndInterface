from tkinter import *
from tkinter import messagebox
from tkinter import filedialog
from tkinter import ttk
from PIL import Image, ImageTk
from datetime import datetime
from matplotlib import font_manager as fm
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import threading
import random
import re
import os


class ComGui():
    def __init__(self, parent_frame, serial, data):
        self.parent_frame = parent_frame
        self.serial = serial
        self.data = data
        
        for node_id in self.data.nodes.keys(): 
            self.data.nodes[node_id] = 'Working'
        
        self.serial_txt_frame = Frame(self.parent_frame, bg='#fe705d')
        
        self.serial_txt = Label(self.serial_txt_frame, text='Serial Setup', 
                                fg='white', bg='#fe705d', font=("Inter", 13, "bold"))
        
        self.serial_frame = Frame(self.parent_frame, bg='#eff0f7', 
                                   highlightbackground='#fe705d',   
                                   highlightthickness=2)

        self.label_com = Label(self.serial_frame, bg='#eff0f7', text="Available Port(s):", 
                               font=("Inter", 11))

        self.label_bd = Label(self.serial_frame, bg='#eff0f7', text="Baud Rate:", 
                              font=("Inter", 11))    
    
        self.ComOptionMenu()
        self.BaudOptionMenu()
        
        self.btn_refresh = Button(self.serial_frame, text="Refresh", width=10, 
                                  font=("Inter", 11), bg='#eff0f7',
                                  command=self.com_refresh)
        self.btn_connect = Button(self.serial_frame, text="Connect", width=10,
                                  font=("Inter", 11), bg='#eff0f7',
                                  state="disabled", 
                                  command=self.serial_connect)
        
        self.separator1 = ttk.Separator(self.serial_frame, orient="horizontal")
                                        
        self.sync_label = Label(self.serial_frame, text="Sync Status: ", 
                                font=("Inter", 11), bg='#eff0f7')
        self.sync_status = Label(self.serial_frame, text="..Sync..", 
                                 bg="#eff0f7", fg="orange", font=("Inter", 11), 
                                 width=10)
        self.layout()


    def ComOptionMenu(self):
        
        # Generate the list of available coms
        self.serial.getCOMList()
        
        self.selected_com = StringVar()
        self.selected_com.set(self.serial.com_list[0])
        self.drop_com = OptionMenu(self.serial_frame, self.selected_com, 
                                   *self.serial.com_list,
                                   command=self.connect_ctrl)
        self.drop_com.config(width=10, bg='#eff0f7', font=('Inter', 11))
        self.drop_com['menu'].config(font=('Inter', 11), bg='#eff0f7')

        
    def BaudOptionMenu(self):
        self.selected_bd = StringVar()
        baud_rates = ["-",
                     "300",
                     "600",
                     "1200",
                     "2400",
                     "4800",
                     "9600",
                     "14400",
                     "19200",
                     "28800",
                     "38400",
                     "56000",
                     "57600",
                     "115200",
                     "128000",
                     "256000"]
        self.selected_bd.set(baud_rates[0])
        self.drop_baud= OptionMenu(self.serial_frame, self.selected_bd, *baud_rates, 
                                   command=self.connect_ctrl)
        self.drop_baud.config(width=10, bg='#eff0f7', font=('Inter', 11))
        self.drop_baud['menu'].config(font=('Inter', 11), bg='#eff0f7')

    def layout(self):
        self.serial_txt_frame.place(x=5, y=10, width=280, height=30)
        self.serial_txt.place(x=90, y=1)
        self.serial_frame.place(x=5, y=35, width=280, height=150)
        
        self.label_com.place(x=5, y=7)
        self.label_bd.place(x=5, y=42)
        
        self.drop_com.place(x=150, y=5)
        self.drop_baud.place(x=150, y=40)
        
        self.btn_refresh.place(x=20, y=77)
        self.btn_connect.place(x=155, y=77)
        
        self.separator1.place(x=5, y=114, width=265)
        
        self.sync_label.place(x=30, y=120)
        self.sync_status.place(x=150, y=120)
        
        self.plot = PlotGui(self.parent_frame)
        self.logger = LoggerGui(self.parent_frame)
        self.datacollect = DataCollectGui(self)
        
        
    def connect_ctrl(self, other):
        if "-" in self.selected_com.get() or "-" in self.selected_bd.get():
            self.btn_connect["state"] = "disable"
        else:
            self.btn_connect["state"] = "active"
    
    def com_refresh(self):
        self.drop_com.destroy()
        self.ComOptionMenu()
        self.drop_com.place(x=150, y=5)
        self.connect_ctrl([])

    def serial_connect(self):
        if self.btn_connect["text"] in "Connect":
            # Start the connection
            self.serial.SerialOpen(self)
            if self.serial.ser.status:
                self.btn_connect["text"] = "Disconnect"
                self.btn_refresh["state"] = "disable"
                self.drop_baud["state"] = "disable"
                self.drop_com["state"] = "disable" 
                InfoMsg = f"Successful UART connection using {self.selected_com.get()}"
                messagebox.showinfo("info", InfoMsg)
            
                self.serial.t1 = threading.Thread(
                    target = self.serial.SerialSync, args=(self,), daemon=True)
                self.serial.t1.start()
                
                self.serial.monitor_thread = threading.Thread(
                    target= self.serial.Monitor_Connection, args=(self,), daemon=True)
                self.serial.monitor_thread.start()
            else:
                ErrorMsg = f"Failure To establish UART connection using {self.selected_com.get()}"
                messagebox.showerror("error", ErrorMsg)
        else:
            self.serial.ser.write(self.data.disconnect_out.encode())
            self.serial.threading = False
            
            # Start closing the connection
            self.serial.SerialClose()   
            self.data.ClearData()
            
            InfoMsg = f"UART connection using {self.selected_com.get()} is closed"
            messagebox.showinfo("info", InfoMsg)
            
            self.datacollect.clt_m1.config(state='normal')
            self.datacollect.clt_m2.config(state='normal')
            self.datacollect.Load_fre.config(state='normal')
        
            self.btn_connect["text"] = "Connect"
            self.sync_status["text"] = "..Sync.."
            self.sync_status["fg"] = "orange"
            self.btn_refresh["state"] = "active"
            self.drop_baud["state"] = "active"
            self.drop_com["state"] = "active"
            self.datacollect.load_status["text"] = "Pending..."
            self.datacollect.load_status["fg"] = "orange"
            self.datacollect.node_status_txt["text"] = "Active Node Status"
            self.datacollect.node_status["text"] = "Pending..."
            self.datacollect.node_status["fg"] = "orange"
            self.datacollect.complete_nodes["text"] = "Pending..."
            self.datacollect.complete_nodes["fg"] = "orange"
            self.datacollect.ACK_status["text"] = "Pending..."
            self.datacollect.ACK_status["fg"] = "orange"
            self.datacollect.time_cal_status["text"] = "Pending..."
            self.datacollect.time_cal_status["fg"] = "orange"
            self.datacollect.freID_status["text"] = "Pending..."
            self.datacollect.freID_status["fg"] = "orange"
            self.datacollect.data_rec_status["text"] = "Pending..."
            self.datacollect.data_rec_status["fg"] = "orange"
            self.datacollect.data_save_status["text"] = "Pending..."
            self.datacollect.data_save_status["fg"] = "orange"
            self.datacollect.btn_collection["text"] = "Start Collection"
            self.datacollect.btn_collection["bg"] = "#eff0f7"
            self.datacollect.btn_collection["fg"] = "black"
                   
class DataCollectGui():
    def __init__(self, com_gui):
        self.parent_frame = com_gui.parent_frame
        self.data = com_gui.data
        self.serial = com_gui.serial
        self.logger = com_gui.logger
        self.plot = com_gui.plot
        self.com = com_gui
        
        downarrow = Image.open("images/down_arrow.png")
        resized_arrow = downarrow.resize((30, 30), Image.LANCZOS)
        self.resized_arrow_var = ImageTk.PhotoImage(resized_arrow)
        
        self.txt_frame1 = Frame(self.parent_frame, bg='#6b59cd')
        
        self.txt1 = Label(self.txt_frame1, text='Data Capture Config.', 
                                fg='white', bg='#6b59cd', font=("Inter", 13, "bold"))
        
        self.data_collect_frame = Frame(self.parent_frame, bg='#eff0f7', 
                                        highlightbackground='#6b59cd',   
                                        highlightthickness=2)
        self.label_cm = Label(self.data_collect_frame, bg='#eff0f7', text="Collection Method:", 
                               font=("Inter", 11))
        
        self.label_node = Label(self.data_collect_frame, bg='#eff0f7', text="Select Node:", 
                               font=("Inter", 11))
        
        self.clt_method = StringVar(value="All Nodes")
        self.clt_m1 = Radiobutton(self.data_collect_frame, text="All Nodes", variable=self.clt_method, 
                                  value="All Nodes", bg='#eff0f7', font=("Inter", 11), command=self.method_ctrl)
        self.clt_m2 = Radiobutton(self.data_collect_frame, text="Single Node", variable=self.clt_method, 
                                  value="Single Node", bg='#eff0f7', font=("Inter", 11), command=self.method_ctrl)
        self.NodeOptionMenu()
        
        # state='disabled
        self.Load_fre = Button(self.data_collect_frame, text="Load Frequencies", 
                               bg='#eff0f7', font=("Inter", 11), command=self.load_file)
        
        self.load_status = Label(self.data_collect_frame, bg='#eff0f7', fg="orange", 
                                text="Pending...", font=("Inter", 11) )
        
        self.btn_collection = Button(self.data_collect_frame, text="Start Collection", 
                                     bg='#eff0f7', font=("Inter", 11), command=self.collection_ctrl)
        
        
        self.txt_frame2 = Frame(self.parent_frame, bg='#008081')
        self.txt2 = Label(self.txt_frame2, text='Track Radio Collection', 
                                fg='white', bg='#008081', font=("Inter", 13, "bold"))
        self.collect_status_frame = Frame(self.parent_frame, bg='#eff0f7', 
                                        highlightbackground='#008081',   
                                        highlightthickness=2)
        
        self.avaliable_nodes_txt = Label(self.collect_status_frame, bg='#eff0f7',
                                     text="All Node(s) in Field", font=("Inter", 11))
        self.avaliable_nodes = Label(self.collect_status_frame, bg='#eff0f7',
                                     text=', '.join(map(str, self.data.nodes.keys())), 
                                     font=("Inter", 11))
        self.separator1 = ttk.Separator(self.collect_status_frame, orient="horizontal")
        
        self.complete_nodes_txt = Label(self.collect_status_frame, bg='#eff0f7',
                                     text="Completed Node(s)", font=("Inter", 11))
        self.complete_nodes = Label(self.collect_status_frame, bg='#eff0f7', fg="orange",
                                     text="Pending", font=("Inter", 11))
        self.separator2 = ttk.Separator(self.collect_status_frame, orient="horizontal")
        self.node_status_txt = Label(self.collect_status_frame, bg='#eff0f7',
                                     text="Active Node Status", font=("Inter", 11))
        
        self.node_status = Label(self.collect_status_frame, bg='#eff0f7', fg="orange",
                                     text="Pending", font=("Inter", 11))
        self.separator3 = ttk.Separator(self.collect_status_frame, orient="horizontal")
        
        self.ACK_Label_text = Label(self.collect_status_frame, text="ACK Rec?", 
                                    bg='#eff0f7', font=("Inter", 11))
        self.ACk_frame = Frame(self.collect_status_frame, bg='#eff0f7', highlightthickness=0)
        self.ACK_status = Label(self.ACk_frame, bg='#eff0f7', fg="orange",
                                     text="Pending...", font=("Inter", 11))
        
        self.downarrow1 = Label(self.collect_status_frame, image=self.resized_arrow_var, bg='#eff0f7')
        
        self.time_cal_text = Label(self.collect_status_frame, text="Time Cal?", 
                                    bg='#eff0f7', font=("Inter", 11))
        self.time_frame = Frame(self.collect_status_frame, bg='#eff0f7', highlightthickness=0)
        self.time_cal_status = Label(self.time_frame, bg="#eff0f7", fg="orange", 
                                text="Pending...", font=("Inter", 11))
        
        self.downarrow2 = Label(self.collect_status_frame, image=self.resized_arrow_var, bg='#eff0f7')
        
        self.freID_text = Label(self.collect_status_frame, text="Freq IDs TX?", 
                                    bg='#eff0f7', font=("Inter", 11))
        self.freq_frame = Frame(self.collect_status_frame, bg='#eff0f7', highlightthickness=0)
        self.freID_status = Label(self.freq_frame, bg="#eff0f7", fg="orange", 
                                text="Pending...", font=("Inter", 11))
        
        self.downarrow3 = Label(self.collect_status_frame, image=self.resized_arrow_var, bg='#eff0f7')
        
        self.data_rec_text = Label(self.collect_status_frame, text="Data Rec?", 
                                    bg='#eff0f7', font=("Inter", 11))
        self.data_rec_frame = Frame(self.collect_status_frame, bg='#eff0f7', highlightthickness=0)
        self.data_rec_status = Label(self.data_rec_frame, bg="#eff0f7", fg="orange", 
                                text="Pending...", font=("Inter", 11))
        self.downarrow4 = Label(self.collect_status_frame, image=self.resized_arrow_var, bg='#eff0f7')
        
        self.data_save_text = Label(self.collect_status_frame, text="Data Saved?", 
                                    bg='#eff0f7', font=("Inter", 11))
        self.data_save_frame = Frame(self.collect_status_frame, bg='#eff0f7', highlightthickness=0)
        self.data_save_status = Label(self.data_save_frame, bg="#eff0f7", fg="orange", 
                                      text="Pending...", font=("Inter", 11))
        self.layout()
    
    def collection_ctrl(self):
        if "Start Collection" in self.btn_collection["text"]:
            if self.data.sync_ok:
                if 'Working' in self.data.nodes.values():
                    if len(self.data.fre_IDs) != 0:
                        if not self.serial.ser.status: 
                            self.serial.SerialOpen(self.com)
                        self.btn_collection["text"] = "End Collection"
                        self.btn_collection["bg"] = "#6b994d"
                        self.btn_collection["fg"] = "white"
                        
                        self.clt_m1.config(state='disabled')
                        self.clt_m2.config(state='disabled')
                        self.drop_node.config(state='disabled')
                        self.Load_fre.config(state='disabled')
                        
                        self.data.working_nodes = [key for key, value in self.data.nodes.items() 
                                                   if value == 'Working']
                        
                        self.serial.t1 = threading.Thread(target=self.serial.SerialStream, 
                                                          args=(self,), daemon=True)
                        self.serial.t1.start()
                        
                    else:
                        ErrorMsg = "No frequencies loaded."
                        messagebox.showerror("Error", ErrorMsg)
                else:
                    ErrorMsg = "No sensor nodes selected."
                    messagebox.showerror("Error", ErrorMsg)
            else:
                ErrorMsg = "Please configure the serial setup and ensure synchronization is complete before starting data collection."
                messagebox.showerror("Error", ErrorMsg)
                    
        else:
            self.serial.threading = False
            self.com.serial_connect()
            self.btn_collection["text"] = "Start Collection"
            self.btn_collection["bg"] = "#eff0f7"
            self.btn_collection["fg"] = "black"
            
            self.clt_m1.config(state='normal')
            self.clt_m2.config(state='normal')
            self.method_ctrl()
            if "Single Node" in self.clt_method.get(): 
                self.node_selection([])
            self.Load_fre.config(state='normal')
    
    def NodeOptionMenu(self):
        # Generate the list of available nodes
        self.node_list = ['-'] + [f'Node {num}' for num in self.data.nodes.keys()]
        
        self.selected_node = StringVar()
        self.selected_node.set(self.node_list[0])
        self.drop_node = OptionMenu(self.data_collect_frame, self.selected_node, 
                                   *self.node_list, command=self.node_selection)
        self.drop_node.config(state='disabled', width=10, bg='#eff0f7', font=('Inter', 11))
        self.drop_node['menu'].config(font=('Inter', 11), bg='#eff0f7')
        
    
    def method_ctrl(self):
        if "All Nodes" in self.clt_method.get(): 
            self.selected_node.set(self.node_list[0])
            self.drop_node.config(state=DISABLED)
            for node_id in self.data.nodes.keys(): 
                self.data.nodes[node_id] = 'Working'
        else:
            self.drop_node.config(state=NORMAL)
            self.data.nodes = {re.search(r'\d+', node['name']).group(): 'NotSelected' 
                              for node in self.data.config}
            
            
    def node_selection(self, other):
        self.data.nodes = {re.search(r'\d+', node['name']).group(): 'NotSelected' 
                          for node in self.data.config}
        selected_id = self.selected_node.get()
        if "-" not in selected_id:
            numid = re.search(r'\d+', selected_id).group()
            self.data.nodes[numid] = 'Working'
        
    
    def layout(self):
        self.txt_frame1.place(x=5, y=195, width=280, height=30)
        self.txt1.place(x=55, y=3)
        self.data_collect_frame.place(x=5, y=225, width=280, height=185)
        
        self.clt_m1.place(x=5, y=5)
        self.clt_m2.place(x=150, y=5)
        
        self.label_node.place(x=5, y=43)
        self.drop_node.place(x=150, y=40)
    
        self.Load_fre.place(relwidth=1.0, y=80)
        self.load_status.place(relwidth=1.0, y=115)
        
        self.btn_collection.place(relwidth=1.0, y=145)
        
        
        self.txt_frame2.place(x=5, y=420, width=280, height=30)
        self.txt2.place(x=50-3, y=3+1)
        self.collect_status_frame.place(x=5, y=450, width=280, height=425)
        
        self.avaliable_nodes_txt.place(relwidth=1.0, y=5)
        self.avaliable_nodes.place(relwidth=1.0, y=27)
        self.separator1.place(x=5, y=55, width=265)
        
        self.complete_nodes_txt.place(relwidth=1.0, y=60)
        self.complete_nodes.place(relwidth=1.0, y=82)
        self.separator2.place(x=5, y=110, width=265)
        
        self.node_status_txt.place(relwidth=1.0, y=115)
        self.node_status.place(relwidth=1.0, y=137)
        self.separator3.place(x=5, y=165, width=265)
        
        self.ACK_Label_text.place(x=18, y=170)
        self.ACk_frame.place(x=97, y=170, width=179, height=22)
        self.ACK_status.place(relwidth=1.0, y=0)
        self.downarrow1.place(x=35, y=190)
        
        self.time_cal_text.place(x=18, y=225)
        self.time_frame.place(x=97, y=225, width=179, height=22)
        self.time_cal_status.place(relwidth=1.0, y=0)
        self.downarrow2.place(x=35, y=245)
        
        self.freID_text.place(x=8, y=280)
        self.freq_frame.place(x=97, y=280, width=179, height=22)
        self.freID_status.place(relwidth=1.0, y=0)
        self.downarrow3.place(x=35, y=300)
        
        self.data_rec_text.place(x=18, y=335)
        self.data_rec_frame.place(x=97, y=335, width=179, height=22)
        self.data_rec_status.place(relwidth=1.0, y=0)
        self.downarrow4.place(x=35, y=355)
        
        
        self.data_save_text.place(x=10, y=390)
        self.data_save_frame.place(x=97, y=390, width=179, height=22)
        self.data_save_status.place(relwidth=1.0, y=0)
    
        
    def load_file(self):
        file_path = filedialog.askopenfilename(
            title="Select an Excel file for Frequencies",
            filetypes=(("Excel files", "*.xlsx *.xls"), ("All files", "*.*"))
        )
        if file_path:
            try:
                # Read the Excel file
                df = pd.read_excel(file_path)
                file_length = len(df) 
                if file_length == 3: 
                    # 'al' 'fz' 's'
                    if df.iloc[0, 0] == 97 and df.iloc[1, 0] == 108 \
                       and df.iloc[2, 0] == 102:
                        self.load_status["text"] = "CMD to send all IDs"
                        self.load_status["fg"] = "green"
                        self.data.fre_IDs = df.to_numpy()
                    else:
                        self.load_status["text"] = "Error loading all IDs."
                        self.load_status["fg"] = "red"
                else: 
                    self.load_status["text"] = f"{file_length} IDs loaded"
                    self.load_status["fg"] = "green"
                    self.data.fre_IDs = df.to_numpy()
                
                # print(self.data.fre_IDs)
            except Exception as e:
                print(e)
                self.load_status["text"] = f"Error in loading IDs"
                self.load_status["fg"] = "red"  

class PlotGui:
    def __init__(self, parent_frame):
        self.parent_frame = parent_frame
        self.legend = None
        self.lines_ax1 = []  # To store lines for ax1
        self.lines_ax2 = []  # To store lines for ax2
        
        # Create a custom font object for Matplotlib
        inter_font_path = "font/Inter_18pt-Regular.ttf"
        self.custom_font = fm.FontProperties(fname=inter_font_path, size=7.5)
        self.legend_font = fm.FontProperties(fname=inter_font_path, size=7)
        
        
        self.plot_frame = Frame(self.parent_frame, bg='#eff0f7', 
                                highlightbackground='#797982', 
                                highlightcolor='#797982',
                                highlightthickness=2)
        self.figure_frame = Frame(self.plot_frame, bg='#eff0f7', 
                                highlightthickness=0)
        self.button_frame = Frame(self.plot_frame, bg='#eff0f7', 
                                highlightthickness=0)
        # Add Save Fig button
        self.Save_fig = Button( self.button_frame, text="Export Figure", state="active", 
                               bg='#eff0f7', font=("Inter", 11), command=self.save_figure)
        # Add Data
        self.add_new_data = Button(self.button_frame, text="Import Data to Compare", state="active", 
                                   bg='#eff0f7', font=("Inter", 11), command=self.add_data)
        self.delete_data_txt = Label(self.button_frame, text="Select Line to Remove:", 
                                     bg='#eff0f7', font=("Inter", 11))
        self.LineOptionMenu()
        self.layout()
        # Draw the plot
        self.draw_plot()  
        
    def layout(self):
        self.plot_frame.place(x=295, y=10, width=605, height=865)
        self.figure_frame.place(x=5, y=0, width=595, height=825)
        self.button_frame.place(x=0, y=825, width=595, height=35)
        self.Save_fig.place(x=10, y=0)
        self.add_new_data.place(x=145, y=0)
        self.delete_data_txt.place(x=335, y=3)
        
    def LineOptionMenu(self):
        if len(self.lines_ax1)>0: 
            line_list = ['-'] + [line.get_label() for line in self.lines_ax1] + ['Clear All Lines']
        else:
            line_list = ['-'] + [line.get_label() for line in self.lines_ax1]
        
        self.selected_line = StringVar()
        self.selected_line.set(line_list[0])
        self.drop_line = OptionMenu(self.button_frame, self.selected_line, 
                                   *line_list, command=self.delete_line_by_label)
        self.drop_line['menu'].config(font=('Inter', 11), bg='#eff0f7')
        self.drop_line.place(x=490, y=0, width=110)
        

    def draw_plot(self):
        # Create a figure with 2 subplots that share both x and y axes
        self.fig, (self.ax1, self.ax2) = plt.subplots(nrows=2, ncols=1, sharex=True)
        self.fig.subplots_adjust(top=0.99, bottom=0.2, left=0.13, right=0.99, hspace=0.05)
        
        self.fig.set_facecolor("#eff0f7") 
        self.ax1.set_facecolor("#eff0f7")
        self.ax2.set_facecolor("#eff0f7")
        
        self.ax1.spines['top'].set_visible(False)
        self.ax1.spines['right'].set_visible(False)
        self.ax1.spines['left'].set_visible(False)
        self.ax2.spines['top'].set_visible(False)
        self.ax2.spines['right'].set_visible(False)
        self.ax2.spines['left'].set_visible(False)

        # Set titles and labels
        self.ax1.set_ylabel("Magnitude Ratio (dB)", fontproperties=self.custom_font, labelpad=0.5)
        self.ax2.set_ylabel("Absolute Phase Difference (°)", fontproperties=self.custom_font, labelpad=0.5)
        self.ax1.set_xscale("log")
        self.ax2.set_xscale("log")
        self.fig.supxlabel("Frequency (Hz)", fontproperties=self.custom_font, y=0.13)  
        
        for label in self.ax1.get_yticklabels() + self.ax2.get_yticklabels() + self.ax2.get_xticklabels(): 
            label.set_fontproperties(self.custom_font)
            
        tick_params = {'color': 'grey', 'width': 0.5}
        self.ax1.tick_params(axis='y', which='both', **tick_params)
        self.ax2.tick_params(axis='y', which='both', **tick_params)
        
        self.ax1.yaxis.grid(True, color='grey', linewidth=0.5)
        self.ax2.yaxis.grid(True, color='grey', linewidth=0.5)
    
        # Create annotation box for displaying hover information
        self.annot = self.fig.text(0, 0, "", bbox=dict(boxstyle="round", fc="w"), ha="center", 
                                   fontproperties=self.custom_font, visible=False)

        # Connect the hover function to motion_notify_event
        self.fig.canvas.mpl_connect("motion_notify_event", self.hover)

        # Embed the Matplotlib figure in Tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.figure_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill='both', expand=True)

    def update_plot(self, new_x, new_y1, new_y2, label):
        """Add new lines to the plot based on new data."""
        
        color = (random.random(), random.random(), random.random()) 
        
        # Add a new line to ax1
        new_line1, = self.ax1.plot(new_x, new_y1, linestyle='-', linewidth=1.5, color=color, label=label)
                                  #  marker='o', markersize=1, color=color, label=label)
        self.lines_ax1.append(new_line1)

        # Add a new line to ax2
        new_line2, = self.ax2.plot(new_x, new_y2, linestyle='-', linewidth=1.5, color=color, label=label)  
                                  #  marker='o', markersize=1, 
        self.lines_ax2.append(new_line2)

        # Rescale axes to fit new data
        self.ax1.relim()
        self.ax1.autoscale_view()
        self.ax2.relim()
        self.ax2.autoscale_view()
        
        # Remove the previous legend if it exists
        if self.legend:
            self.legend.remove()
            self.legend = None
        
        labels = [line.get_label() for line in self.lines_ax1]
        
        self.legend = self.fig.legend(self.lines_ax1, labels, loc='upper center', 
                                      bbox_to_anchor=(0.5, 0.13), ncol=3, 
                                      prop=self.legend_font, columnspacing=0.95)

        self.LineOptionMenu()
        
        # Redraw the canvas to reflect the new lines
        self.canvas.draw()
    
    def delete_line_by_label(self, other):
        """Delete a line from the plot based on its label name."""
        label_name = self.selected_line.get()
     
        if "-" not in label_name:
            if "Clear All Lines" in label_name:
                for line in self.lines_ax1:
                    line.remove()  # Remove the line from the axes
                self.lines_ax1.clear()  # Clear the list after removing all lines
    
                # Iterate over the lines in ax2 and remove the matching line
                for line in self.lines_ax2:
                    line.remove()  # Remove the line from the axes
                self.lines_ax2.clear()  # Clear the list after removing all lines
                       
            else:
                for line in self.lines_ax1:
                    if line.get_label() == label_name:
                        line.remove()  # Remove the line from the axes
                        self.lines_ax1.remove(line)  # Remove the line from the list
                        break  # Exit the loop once the line is found
            
                # Iterate over the lines in ax2 and remove the matching line
                for line in self.lines_ax2:
                    if line.get_label() == label_name:
                        line.remove()  # Remove the line from the axes
                        self.lines_ax2.remove(line)  # Remove the line from the list
                        break  # Exit the loop once the line is found
        
            # Update the legend after removing a line
            if self.legend:
                self.legend.remove()
                self.legend = None
        
            # Update the labels for the remaining lines
            labels = [line.get_label() for line in self.lines_ax1]
            if labels:  # Only create a legend if there are lines remaining
                self.legend = self.fig.legend(self.lines_ax1, labels, loc='upper center', 
                                              bbox_to_anchor=(0.5, 0.13), ncol=3, 
                                              prop=self.legend_font, columnspacing=0.95)
            # Rescale axes to fit new data
            self.ax1.relim()
            self.ax1.autoscale_view()
            self.ax2.relim()
            self.ax2.autoscale_view()
        
            self.LineOptionMenu()
            
            # Redraw the canvas to reflect the changes
            self.canvas.draw()

        
    def add_data(self):
        file_path = filedialog.askopenfilename( 
            title="Select a collected file to add",
            filetypes=(("CSV and Text files", "*.csv *.txt"),)  # Show both CSV and TXT files only
            )
        if file_path:
            try:
                file_extension = os.path.splitext(file_path)[-1].lower()

                if file_extension == '.csv':
                    fre_idx = pd.read_csv(file_path, usecols=["fre_idx"]).squeeze()
                    mag = pd.read_csv(file_path, usecols=["mag (dig)"]).squeeze()
                    phs = pd.read_csv(file_path, usecols=["phs (dig)"]).squeeze()
                    fre, mag_dB, phs_deg = self.calculate_freq_mag_phase(fre_idx, mag, phs)
                    self.update_plot(fre, mag_dB, phs_deg,  self.create_label(file_path, file_extension)) 
                elif file_extension == '.txt':
                    fre_idx = pd.read_csv(file_path, delimiter=r'\s+', header=None, usecols=[0]).squeeze()
                    mag = pd.read_csv(file_path, delimiter=r'\s+', header=None, usecols=[2]).squeeze()
                    phs = pd.read_csv(file_path, delimiter=r'\s+', header=None, usecols=[3]).squeeze()
                    fre, mag_dB, phs_deg = self.calculate_freq_mag_phase(fre_idx, mag, phs)
                    self.update_plot(fre, mag_dB, phs_deg,  self.create_label(file_path, file_extension)) 
                else:
                    ErrorMsg = "Unsupported File Type."
                    messagebox.showerror("Error", ErrorMsg)
            except Exception as e:
                print(e)
                ErrorMsg = f"Error in loading file: {e}."
                messagebox.showerror("Error", ErrorMsg)
    
    
    def create_label(self, file_path, filetype):
        # Split the path
        parts = file_path.split("/")
        
        if ".csv" in filetype:
            parts = file_path.split("/")
            filename = parts[-1]
            node_number = filename.split('-')[0][1:]
            node = f"Node {node_number} "
            date_txt = filename.split('-')[1] 
            time_txt = filename.split('-')[2] 
            
            date_parts = date_txt.split('_')
            time_parts = time_txt.split('_')
            label = node + "".join(date_parts) + "_" + "".join(time_parts[:2])
        else:  # txt file
            # Determine if it's lab or field data
            if "Lab" in parts:
                # Extract relevant parts for lab data
                label = f"{parts[-3]} {parts[-2]} {os.path.splitext(parts[-1])[0]}"
            elif "UG nodes" in parts:
                node_mapping = {
                    'EP_WN': 3,
                    'EP_ON': 5,
                    'LP_WN': 7,
                    'LP_ON': 9,
                    }
                part1 = parts[-3]
                part2 = parts[-2].split('_')[0]  
                node_number = node_mapping.get(f"{part1}_{part2}", None) 
                part3 = os.path.splitext(parts[-1])[0].split('_')[1]
                part4 = os.path.splitext(parts[-1])[0].split('_')[2][:4]
                label = f"Node {node_number} {part3}_{part4}"
        
        return label
    
        
    def calculate_freq_mag_phase(self, IDs, mag, phs):
        fre_size = len(IDs)
        # Preallocate the frequency array with zeros
        fre = np.zeros(fre_size)
        # For i <= 10, calculate frequencies as 100 * i
        fre[IDs <= 10] = 100 * IDs[IDs <= 10]
        # For 10 < i <= 110, use a logarithmic scale starting from 1000 Hz
        fre[(IDs > 10) & (IDs <= 110)] = 10 ** (0.03 * (IDs[(IDs > 10) & (IDs <= 110)] - 10)) * 1000
        # For i > 110, use a logarithmic scale starting from 1 MHz (1e6 Hz)
        fre[IDs > 110] = 10 ** (0.003 * (IDs[IDs > 110] - 110)) * 1e6
        
        
        mag = (mag / 4095 * 3.3 / 1.8 - 1.8) / 0.06
        
        phs = (phs / 4095 * 3.3 / 1.8 - 0.9) / (-0.01) + 90
        phs = phs + abs(phs.min())
    
        return fre, mag, phs
    
    def hover(self, event):
        """Hover function to display data point information dynamically."""
        vis = self.annot.get_visible()

        for ax, lines in [(self.ax1, self.lines_ax1), (self.ax2, self.lines_ax2)]:
            for line in lines:
                cont, ind = line.contains(event)
                if cont:
                    self.update_annot(line, ind, ax)
                    return
        if vis:
            self.annot.set_visible(False)
            self.fig.canvas.draw_idle()

    def update_annot(self, line, ind, ax):
        """Update annotation with data point information."""
        x_data, y_data = line.get_data()
        x_point = x_data[ind["ind"][0]]
        y_point = y_data[ind["ind"][0]]

        # Update annotation text and position
        self.annot.set_text(f"f={x_point/1000:.2f}k, Y={y_point:.2f}")
        self.annot.set_position((x_point, y_point))  # Adjust for visibility
        self.annot.set_visible(True)
        self.annot.set_transform(ax.transData)
        self.fig.canvas.draw_idle()
    
    def save_figure(self):
        folder_path = StringVar()
        folder_selected = filedialog.askdirectory()
        
        # Check if the folder selection was canceled
        if not folder_selected:
            return 
        
        folder_path.set(folder_selected)
        
        now = datetime.now()
        filename = (f"{(now.year)%100:02d}_"
                    f"{now.month:02d}_"
                    f"{now.day:02d}-"
                    f"{now.hour:02d}_"
                    f"{now.minute:02d}_"
                    f"{now.second:02d}.png")
        
        folder = folder_path.get()
        complete_path = f"{folder}/{filename}"
        
        try:
            self.fig.savefig(complete_path)
        except Exception as e:
            print(e)
    
    def close_plot(self):
        """Close the plot and release all resources."""
        # Remove the figure canvas
        if hasattr(self, 'canvas'):
            self.canvas.get_tk_widget().destroy()

        # Close the figure
        plt.close(self.fig)
        
    
class LoggerGui():
    def __init__(self, parent_frame):
        self.parent_frame = parent_frame
        
        self.logeer_frame = Frame(self.parent_frame, bg="#eff0f7", highlightthickness=0)
        
        self.logger = Text(self.logeer_frame, bg='#eff0f7', relief="solid", font=("Electra", 10))
        
        self.scrollbar = Scrollbar(self.logeer_frame, command=self.logger.yview)
        self.logger.configure(yscrollcommand=self.scrollbar.set)
        
        self.auto_scroll = BooleanVar(value=True)
        self.clear_btn =Button(self.logeer_frame, text="Clear Logs", state = "active",
                               bg='#eff0f7', font=("Inter", 11), command=self.ClearLoggerInfo)
        self.chk_auto_scroll = Checkbutton(self.logeer_frame, text="Auto-Scroll On",
                                           bg='#eff0f7', font=("Inter", 11), variable=self.auto_scroll)
        
        self.Save_Info = Button(self.logeer_frame, text="Save Logs", state="active",
                                bg='#eff0f7', font=("Inter", 11), command=self.SaveLoggerInfo)
        
        self.layout()
        
        
    def layout(self):
        self.logeer_frame.place(x=910, y=10, width=420, height=865)
        self.logger.place(x=0, y=0, width=400, height=815)
        
        self.scrollbar.place(x=400, y=0, width=10, height=815)
        self.chk_auto_scroll.place(x=0, y=828)
        self.clear_btn.place(x=180, y=825)
        self.Save_Info.place(x=320, y=825)
    
    def ClearLoggerInfo(self):
        self.logger.delete("1.0", END)
        
    def SaveLoggerInfo(self):
        folder_path = StringVar()
        folder_selected = filedialog.askdirectory()
        
        # Check if the folder selection was canceled
        if not folder_selected:
            return 
    
        folder_path.set(folder_selected)
        
        now = datetime.now()
        filename = (f"{(now.year)%100:02d}_"
                    f"{now.month:02d}_"
                    f"{now.day:02d}-"
                    f"{now.hour:02d}_"
                    f"{now.minute:02d}_"
                    f"{now.second:02d}.text")
        
        folder = folder_path.get()
        complete_path = f"{folder}/{filename}"
        
        content = self.logger.get("1.0", "end-1c")
        
        try:
            with open(complete_path, "w") as file:
                file.write(content)
        except Exception as e:
            print(f"Error while saving file: {e}")