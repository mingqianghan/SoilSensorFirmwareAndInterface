# -*- coding: utf-8 -*-
"""
Created on Sat Apr 20 16:47:26 2024

@author: mingqiang
"""

from tkinter import *
from tkinter import messagebox
from tkinter import ttk
import threading
from tkinter import filedialog
from datetime import datetime
import pandas as pd

class RootGUI:
    def __init__(self, serial, data):
        self.root = Tk()
        self.root.title("Serial Communcation")
        self.root.geometry("360x130")
        self.root.config(bg="white")
        self.serial = serial
        self.data = data
        
        self.root.protocol("WM_DELETE_WINDOW", self.close_window)
        
        
    def close_window(self):
       # print("Closing the window and exit")
        self.serial.threading = False
        self.root.destroy()
        self.serial.Close_Com()
                

class ComGui():
    def __init__(self, root, serial, data):
        self.root = root
        self.serial = serial
        self.data = data
        
        self.frame = LabelFrame(root, text="Serail Port Manager", 
                                padx=5, pady=5, bg="white")
        
        self.label_com = Label(self.frame, text = "Avaliable Port(s):",
                               bg="white", width=15, anchor="w")
        
        self.label_bd = Label(self.frame, text = "Baud Rate:",
                               bg="white", width=15, anchor="w")
        self.ComOptionMenu()
        self.BaudOptionMenu()
        
        self.btn_refresh = Button(self.frame, text="Refresh", width=10, 
                                  command=self.com_refresh)
        self.btn_connect = Button(self.frame, text="Connect", width=10,
                                  state="disabled", 
                                  command=self.serial_connect)
        
        self.padx = 20
        self.pady = 5
        self.publish()
    
    
    def ComOptionMenu(self):
        
        # Generate the list of available coms
        self.serial.getCOMList()
        
        self.selected_com = StringVar()
        self.selected_com.set(self.serial.com_list[0])
        self.drop_com = OptionMenu(self.frame, self.selected_com, 
                                   *self.serial.com_list,
                                   command=self.connect_ctrl)
        self.drop_com.config(width=10)
        
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
        self.drop_baud= OptionMenu(self.frame, self.selected_bd, *baud_rates, 
                                   command=self.connect_ctrl)
        self.drop_baud.config(width=10)


    def publish(self):
        self.frame.grid(row=0, column=0, rowspan=2, 
                        columnspan=3, padx=5, pady=8, sticky="new")
        
        self.label_com.grid(row=0, column=0)
        self.label_bd.grid(row=1, column=0)
        
        self.drop_com.grid(row=0, column=1, padx=self.padx, pady=self.pady)
        self.drop_baud.grid(row=1, column=1, padx=self.padx, pady=self.pady)
        
        self.btn_refresh.grid(row=0, column=2)
        self.btn_connect.grid(row=1, column=2)
        
        
        
    def connect_ctrl(self, other):
        if "-" in self.selected_com.get() or "-" in self.selected_bd.get():
            self.btn_connect["state"] = "disable"
        else:
            self.btn_connect["state"] = "active"
    
    def com_refresh(self):
        self.drop_com.destroy()
        self.ComOptionMenu()
        self.drop_com.grid(row=0, column=1, padx=self.padx)
        logic = []
        self.connect_ctrl(logic)
    
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
                
                self.log_text = LoggerGui(self.root)
                self.fieldInfo = FieldInfoGUI(self.root)
                self.conn = ConnGui(self.root, self.serial, self.data, 
                                    self.fieldInfo, self.log_text)
                
                self.serial.t1 = threading.Thread(
                    target = self.serial.SerialSync, args=(self,), daemon=True)
                self.serial.t1.start()
            else:
                ErrorMsg = f"Failure To establish UART connection using {self.selected_com.get()}"
                messagebox.showerror("error", ErrorMsg)
        else:
            self.serial.threading = False
            
            # Start closing the connection
            self.serial.SerialClose()
            self.fieldInfo.FieldInfoGUIClose()
            self.log_text.LoggerGUIClose()
            self.conn.ConnGuiClose()
            
            self.data.ClearData()
            
            InfoMsg = f"UART connection using {self.selected_com.get()} is closed"
            messagebox.showinfo("info", InfoMsg)
            self.btn_connect["text"] = "Connect"
            self.btn_refresh["state"] = "active"
            self.drop_baud["state"] = "active"
            self.drop_com["state"] = "active"
    

class ConnGui():
    def __init__(self, root, serial, data, fieldInfo, logger):
        self.root = root
        self.serial = serial
        self.data = data
        self.fieldInfo = fieldInfo
        self.logger = logger
        
        self.frame = LabelFrame(root, text="Data Collection Manager", 
                                padx=5, pady=5, bg="white", width=60)
        
        self.sync_label = Label(
            self.frame, text="Sync Status: ", bg="white", width=15, anchor="w")
        self.sync_status = Label(
            self.frame, text="..Sync..", bg="white", fg="orange", width=30)
        
        self.allnode_label = Label(
            self.frame, text="UG Node(s): ", bg="white", width=15, anchor="w")
        self.allnode_status = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)

        self.nodedone_label = Label(
            self.frame, text="Node(s) Done: ", bg="white", width=15, anchor="w")
        self.nodedone_status = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)
        
        self.Load_fre = Button(self.frame, text="Load Freq. IDs", 
                               state="disabled",height=2,width=15, 
                               command=self.load_file)
        
        self.load_status = Label(self.frame, bg ="white", fg="orange", 
                                text="Pending...", width=15)
        
        self.btn_start_collection = Button(self.frame, text="Start Collection", 
                                           state="disabled",height=2,width=15, 
                                           command=self.start_collection)
        
        self.btn_stop_collection = Button(self.frame, text="Stop Collection", 
                                           state="disabled",height=2,width=15, 
                                           command=self.stop_collection)
        
        self.separator1 = ttk.Separator(self.frame, orient="horizontal")
        self.separator2 = ttk.Separator(self.frame, orient="horizontal")
        
        self.text_dis = Label(self.frame, text="Node Status:",
                              bg="#D3D3D3", height=2, width=15)
        
        self.Node_status = Label(self.frame, text="Pending...",
                              bg="#D3D3D3", height=2, fg='orange', width=30)
        
        self.ACK_Label_text = Label(
            self.frame, text="ACK Received? ", bg ="white", width=15, anchor="e")
        self.ACK_status = Label(self.frame, bg ="white", fg="orange", 
                                text="Pending...")
        
        self.canvas1 = Canvas(self.frame, width=150, height=30, bg="white",
                             highlightbackground="white", highlightthickness=1)
        
        self.time_cal_text = Label(
            self.frame, text="Time Calibrated? ", bg="white", width=15, anchor="e")
        self.time_cal_status = Label(self.frame, bg="white", fg="orange", 
                                text="Pending...")
        
        self.canvas2 = Canvas(self.frame, width=150, height=30, bg="white",
                             highlightbackground="white", highlightthickness=1)
        
        self.freID_text = Label(
            self.frame, text="Fre. IDs TX? ", bg="white", width=15, anchor="e")
        self.freID_status = Label(self.frame, bg="white", fg="orange", 
                                text="Pending...")
        
        self.canvas4 = Canvas(self.frame, width=150, height=30, bg="white",
                             highlightbackground="white", highlightthickness=1)
        
        self.data_rec_text = Label(
            self.frame, text="Data Received? ", bg="white", width=15, anchor="e")
        self.data_rec_status = Label(self.frame, bg="white", fg="orange", 
                                text="Pending...")
        
        self.canvas3 = Canvas(self.frame, width=150, height=30, bg="white",
                             highlightbackground="white", highlightthickness=1)
        
        self.data_save_text = Label(
            self.frame, text="Data Saved? ", bg="white", width=15, anchor="e")
        self.data_save_status = Label(self.frame, bg="white", fg="orange", 
                                text="Pending...")
        
        self.ConnGuiOpen()
        
        
    def ConnGuiOpen(self):
        self.root.geometry("360x310")
        self.frame.grid(row=6, column=0, rowspan=17, columnspan=2, 
                        padx=5, pady=5, sticky="new")
        
        self.sync_label.grid(row=0, column=0, pady=5)
        self.sync_status.grid(row=0, column=1, pady=5)
        
        self.allnode_label.grid(row=1, column=0, pady=5)
        self.allnode_status.grid(row=1, column=1, pady=5)

        self.nodedone_label.grid(row=2, column=0, pady=5)
        self.nodedone_status.grid(row=2, column=1, pady=5)
        
        self.separator1.grid(row=3, column=0, columnspan=2, pady=5, sticky="ew")
        
        self.Load_fre.grid(row=4, column=0, padx=5, pady=5, sticky="w")
        self.load_status.grid(row=4, column=1, padx=5, pady=5)
        
        self.btn_start_collection.grid(row=5, column=0, 
                                       padx=5, pady=5, sticky="w")
        self.btn_stop_collection.grid(row=5, column=1, 
                                      padx=5, pady=5, sticky="e")
        
        self.separator2.grid(row=6, column=0, columnspan=2, pady=5, sticky="ew")
        
        self.text_dis.grid(row=7, column=0, pady=5)
        self.Node_status.grid(row=7, column=1, pady=5)
        
        self.ACK_Label_text.grid(row=8, column=0, 
                                 padx=60, columnspan=2, sticky='w')
        self.ACK_status.grid(row=8, column=1)
        
        self.canvas1.grid(row=9, column=0, columnspan=2, sticky='w')
        
        self.time_cal_text.grid(row=10, column=0, 
                                padx=60, columnspan=2, sticky='w')
        self.time_cal_status.grid(row=10, column=1)
        
        self.canvas2.grid(row=11, column=0, columnspan=2, sticky='w')
        
        self.freID_text.grid(row=12, column=0, 
                                padx=60, columnspan=2, sticky='w')
        self.freID_status.grid(row=12, column=1)
        
        self.canvas4.grid(row=13, column=0, columnspan=2, sticky='w')
        
        self.data_rec_text.grid(row=14, column=0,
                                padx=60, columnspan=2, sticky='w')
        self.data_rec_status.grid(row=14, column=1)
        
        self.canvas3.grid(row=15, column=0, columnspan=2, sticky='w')
        
        self.data_save_text.grid(row=16, column=0, 
                                 padx=60, columnspan=2, sticky='w')
        self.data_save_status.grid(row=16, column=1) 
        
        self.root.update()
        self.canvas1.create_line(125, 0, 125, 30, arrow=LAST, width=2)
        self.canvas2.create_line(125, 0, 125, 30, arrow=LAST, width=2)
        self.canvas3.create_line(125, 0, 125, 30, arrow=LAST, width=2)
        self.canvas4.create_line(125, 0, 125, 30, arrow=LAST, width=2)
        
        
        
    def ConnGuiClose(self):
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()
        self.root.geometry("360x130")
        
        
    def start_collection(self):
        self.btn_start_collection["state"] = "disabled"
        self.btn_stop_collection["state"] = "active"
        
        self.serial.t1 = threading.Thread(target=self.serial.SerialStream, 
                                          args=(self,), daemon=True)
        self.serial.t1.start()
    
    def stop_collection(self):
        self.Load_fre["state"] = "active"
        self.btn_start_collection["state"] = "disabled"
        self.btn_stop_collection["state"] = "disabled"
        self.serial.threading = False
        
    def load_file(self):
        file_path = filedialog.askopenfilename(
            title="Select an Excel file for Frequency IDs",
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
                        self.Load_fre["state"] = "disable"
                        self.btn_start_collection["state"] = "active"
                        self.data.fre_IDs = df.to_numpy()
                    else:
                        self.load_status["text"] = "Error loading all IDs."
                        self.load_status["fg"] = "red"
                else: 
                    self.load_status["text"] = f"{file_length} IDs loaded"
                    self.load_status["fg"] = "green"
                    self.Load_fre["state"] = "disable"
                    self.btn_start_collection["state"] = "active"
                    self.data.fre_IDs = df.to_numpy()
                
                # print(self.data.fre_IDs)
            except Exception as e:
                self.load_status["text"] = f"Error: {e}"
                self.load_status["fg"] = "red"
                
                

class FieldInfoGUI():
    def __init__(self, root):
        self.root = root
        self.frame = LabelFrame(root, text="Field Information", 
                                padx=5, pady=5, bg="white", width=60)
        self.sitename_label = Label(
            self.frame, text="Location: ", bg="white", width=15, anchor="w")
        self.sitename_dis = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)
        
        self.Latitude_label = Label(
            self.frame, text="Latitude: ", bg="white", width=15, anchor="w")
        self.Latitude_dis = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)
        
        self.Longitude_label = Label(
            self.frame, text="Longitude: ", bg="white", width=15, anchor="w")
        self.Longitude_dis = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)
        
        self.sensor_num_label = Label(self.frame, 
                                      text="Number of sensors: ", 
                                      bg="white", width=15, anchor="w")
        self.sensor_num_dis = Label(
            self.frame, text="...", bg="white", fg="orange", width=30)
        
        self.FieldInfoGUIOpen()
        
    def FieldInfoGUIOpen(self):
        self.frame.grid(row=2, column=0, rowspan=4, columnspan=2, 
                        padx=5, sticky="new")
        
        self.sitename_label.grid(row=0, column=0, pady=5)
        self.sitename_dis.grid(row=0, column=1, pady=5)
        
        self.Latitude_label.grid(row=1, column=0, pady=5)
        self.Latitude_dis.grid(row=1, column=1, pady=5)
        
        self.Longitude_label.grid(row=2, column=0, pady=5)
        self.Longitude_dis.grid(row=2, column=1, pady=5)
        
        self.sensor_num_label.grid(row=3, column=0, pady=5)
        self.sensor_num_dis.grid(row=3, column=1, pady=5)
        
    def  FieldInfoGUIClose(self):
         for widget in self.frame.winfo_children():
             widget.destroy()
         self.frame.destroy()
        
    

class LoggerGui():
    def __init__(self, root):
        self.root = root
        
        self.frame = Frame(root, pady=5, bg="white", width=90)
        
        screen_height = self.root.winfo_screenheight()
        # Assuming roughly 18 pixels per line of text, this is a common height for default font and size
        pixels_per_line = 18
        lines_of_text = screen_height // pixels_per_line
        
        self.logger = Text(self.frame, height=lines_of_text-2, 
                           width=143, 
                           bg='light grey', relief="solid")
        self.scrollbar = Scrollbar(self.frame, command=self.logger.yview)
        
        self.auto_scroll = BooleanVar(value=True)
        self.clear_btn =Button(self.frame, text="Clear Log Info",
                               state = "active", height=2,width=15,
                               command=self.ClearLoggerInfo)
        self.chk_auto_scroll = Checkbutton(self.frame, text="Enable Auto-Scroll",
                                           height=2, variable=self.auto_scroll)
        
        self.logger.configure(yscrollcommand=self.scrollbar.set)
        
        self.Save_Info = Button(self.frame, text="Save Log Info", 
                                state="active",height=2,width=15, 
                                command=self.SaveLoggerInfo)
        
        self.LoggerGUIOpen()
        
        
    def LoggerGUIOpen(self):
        
        self.root.state('zoomed')
        
       # self.root.geometry("1075x595")

        self.frame.grid(row=0, column=3, rowspan=30, columnspan=4, 
                        padx=5, pady=5)
        
        self.logger.grid(row=0, column=0, columnspan=3, sticky="nsew", pady=5)
        self.scrollbar.grid(row=0, column=4, sticky="ns", pady=5)
        self.chk_auto_scroll.grid(row=20, column=0, sticky="w")
        self.clear_btn.grid(row=20, column=1)
        self.Save_Info.grid(row=20, column=2, sticky='e')
    
        
    def LoggerGUIClose(self):
        for widget in self.frame.winfo_children():
            widget.destroy()
        self.frame.destroy()
        self.root.geometry("360x130")
    
    def ClearLoggerInfo(self):
        self.logger.delete("1.0", END)
        
    def SaveLoggerInfo(self):
        folder_path = StringVar()
        folder_selected = filedialog.askdirectory()
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
            print(e)
    
        
if __name__ == "__main__":
    RootGUI()
    ComGui()
    ConnGui()
    FieldInfoGUI()
    LoggerGui()
        