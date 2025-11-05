from tkinter import * 
from PIL import Image, ImageTk
from dotenv import load_dotenv
import os
import time
import json
import re
import ctypes

from HomeGui import HomeGui
from CommCollectGui import ComGui

class AppRoot():
    def __init__(self, Serial, Data):
        load_dotenv()
        self.api_key = os.getenv("API_KEY")
        
        config = self.load_config()
    
        self.serial = Serial
        self.data = Data
        
        self.data.config = config.get("markers")

        self.data.nodes = {re.search(r'\d+', node['name']).group(): 'NotSelected' 
                           for node in self.data.config}
        
        self.root = Tk()
        myappid = 'mycompany.myproduct.subproduct.version' # arbitrary string
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
        self.root.iconbitmap("images/app_icon.ico")
        
        self.root.update()  # Get the screen width and height
        self.root.title('Soil-Weather Interface')
        self.root.geometry(f'{self.root.winfo_screenwidth()}x{self.root.winfo_screenheight()}')
        self.root.state('zoomed')
        self.root.config(bg ='#eff5f6')
    
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        

        # Main container frame to hold different pages
        self.main_frame2 = Frame(self.root, bg='#eff0f7')
        self.main_frame2.place(x=200, y=0, 
                              width=self.root.winfo_screenwidth()-200, 
                              height=self.root.winfo_screenheight())
        
        self.main_frame1 = Frame(self.root, bg='#eff0f7')
        self.main_frame1.place(x=200, y=0, 
                              width=self.root.winfo_screenwidth()-200, 
                              height=self.root.winfo_screenheight())
        
        self.home_frame = None
        self.com_frame = None

        self.open_sidebar()
        
        # Load the initial page
        self.change_frame("Home")
    
    def on_closing(self):
        if hasattr(self.serial, 'ser'):
            if self.serial.ser is not None:
                if self.serial.ser.status: 
                    self.serial.ser.write(self.data.disconnect_out.encode())
                self.serial.ser.close()
   
        self.serial.monitor_thread_running = False
        self.serial.threading = False
        '''
        if self.com_frame is not None:
            self.com_frame.plot.close_plot()
        '''
        if self.home_frame.initialize_ui_id: 
            self.home_frame.parent_frame.after_cancel(self.home_frame.initialize_ui_id)
        if self.show_time_id: 
            self.date_time.after_cancel(self.show_time_id)
        self.root.destroy()
        
    def open_sidebar(self):
        self.sidebar = Frame(self.root, bg='#ffffff')
        self.sidebar.place(x=0, y=0, width=200, height=self.root.winfo_screenheight())

        # Home button
        original_image = Image.open('images/home.png')  
        resized_image = original_image.resize((30, 30), Image.LANCZOS)
        self.HomeImage = ImageTk.PhotoImage(resized_image)
        self.Home = Label(self.sidebar, image=self.HomeImage, bg='#ffffff')
        self.Home.image = self.HomeImage
        self.Home.place(x=10, y=342)
        self.Home_text = Button(self.sidebar, text="Home", bg='#ffffff', font=("Inter", 16, "bold"), bd=0,
                                     cursor='hand2', activebackground='#ffffff', command=lambda: self.change_frame("Home"))
        self.Home_text.place(x=80, y=342)
        
        # Data Comm button
        original_image = Image.open('images/data-exchange.png')  
        resized_image = original_image.resize((30, 30), Image.LANCZOS)
        self.CommCollectImage = ImageTk.PhotoImage(resized_image)
        self.CommCollect = Label(self.sidebar, image=self.CommCollectImage, bg='#ffffff')
        self.CommCollect.image = self.CommCollectImage
        self.CommCollect.place(x=10, y=400)
        self.CommCollect_text = Button(self.sidebar, text="CommCollect", bg='#ffffff', font=("Inter", 16, "bold"), bd=0,
                                  cursor='hand2', activebackground='#ffffff', command=lambda: self.change_frame("CommCollect"))
        self.CommCollect_text.place(x=43, y=400)
        
        
        # Exit button
        original_image = Image.open('images/logout.png')  
        resized_image = original_image.resize((30, 30), Image.LANCZOS)
        self.ExitImage = ImageTk.PhotoImage(resized_image)
        self.Exit = Label(self.sidebar, image=self.ExitImage, bg='#ffffff')
        self.Exit.image = self.ExitImage
        self.Exit.place(x=8, y=458)
        self.Exit_text = Button(self.sidebar, text="Exit", bg='#ffffff', font=("Inter", 16, "bold"), bd=0,
                                cursor='hand2', activebackground='#ffffff', command=self.on_closing)
        self.Exit_text.place(x=85, y=458)
        
        
        # date and Time
        self.clock_image = ImageTk.PhotoImage(file="images/time.png")
        self.date_time_image = Label(self.sidebar, image=self.clock_image, bg="white")
        self.date_time_image.image = self.clock_image
        self.date_time_image.place(x=20, y=150)

        self.date_time = Label(self.sidebar)
        self.date_time.place(x=60, y=145)
        self.show_time()

    def show_time(self):
        self.time = time.strftime("%H:%M:%S")
        self.date = time.strftime('%Y/%m/%d')
        set_text = f"  {self.time} \n {self.date}"
        self.date_time.configure(text=set_text, font=("Inter", 16, "bold"), bd=0, bg="white", fg="black")
        self.show_time_id = self.date_time.after(100, self.show_time)
        
     
    def change_frame(self, page_name):
        # Load the content for the selected page
        if page_name == "Home":
            self.main_frame2.pack_forget()
            if not self.home_frame:
                self.home_frame = HomeGui(self.main_frame1, self.api_key, self.data.config)
            self.main_frame1.lift()
        elif page_name == "CommCollect":
            self.main_frame1.pack_forget()
            if not self.com_frame:
                self.com_frame = ComGui(self.main_frame2, self.serial, self.data)
            self.main_frame2.lift()
    
    
    def load_config(self):
        with open('config.json', 'r') as file:
            config = json.load(file)
        
        return config
    