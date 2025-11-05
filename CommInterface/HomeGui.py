from tkinter import * 
from tkinter import messagebox
from PIL import Image, ImageTk
from io import BytesIO
from tkintermapview import TkinterMapView
from tkcalendar import Calendar
from meteostat import Point, Daily
from tkinter import filedialog
from datetime import *
import requests


from Weather_Summary import WeatherSummary

def new_img_del(img_self):
    try:
        name = img_self.__photo.name
        img_self.__photo.name = None
        img_self.__photo.tk.call("image", "delete", name)
    except Exception:
        pass
ImageTk.PhotoImage.__del__ = new_img_del


def calculate_center(markers):
    """Calculate the average center of all markers."""
    if not markers:
        return None, None
 
    latitudes = [marker['latitude'] for marker in markers]
    longitudes = [marker['longitude'] for marker in markers]
 
    center_latitude = sum(latitudes) / len(latitudes)
    center_longitude = sum(longitudes) / len(longitudes)
 
    return center_latitude, center_longitude


class HomeGui():
    def __init__(self, parent_frame, api_key, config):
        
        self.api_key = api_key
        
        # Node configuration
        self.config = config
        
        # Calculate center
        self.center_lat, self.center_lon = calculate_center(self.config)
        
        self.weather_summary = WeatherSummary(self.center_lat, self.center_lon, self.api_key)

        self.parent_frame = parent_frame
        self.initialize_ui()
        self.initialize_ui_id = self.parent_frame.after(10, self.initialize_ui)
        
    def initialize_ui(self):
        self.draw_current_metrics_box()
        
        self.weather_summary.generate_past_week_summary()
        past_week_summary = self.weather_summary.past_week_summary
        self.draw_summary_metrics_box(past_week_summary, bgset1='#a71666', locy=200, ftitle='Past Week Summary')
        
        self.weather_summary.generate_next_week_summary()
        next_week_summary = self.weather_summary.next_week_summary
        self.draw_summary_metrics_box(next_week_summary, bgset1='#e68815', locy=450, ftitle='Next Week Forecast')
        
        self.save_historical_data()
        
        self.draw_map()
    
    def draw_map(self):

        self.map_widget = TkinterMapView(self.parent_frame, corner_radius=0)
        self.map_widget.place(x=0, y=0, width=1000, height=self.parent_frame.winfo_height())

        
        # Set the map to satellite view
        self.map_widget.set_tile_server("https://mt1.google.com/vt/lyrs=s&x={x}&y={y}&z={z}", max_zoom=22)

        # Set the initial position and zoom level
        self.map_widget.set_position(self.center_lat, self.center_lon)
        self.map_widget.set_zoom(19.5)
        
        # Load and resize marker images
        original_marker_image = Image.open("images/gps.png")  # Replace with your marker image file path
        resized_marker_image = original_marker_image.resize((30, 30), Image.LANCZOS)
        self.marker_icon = ImageTk.PhotoImage(resized_marker_image)
        
        all_info_text = "\n"
        for marker in self.config:
            self.map_widget.set_marker(marker['latitude'], marker['longitude'], 
                                       text=f"{marker['name']}", 
                                       icon=self.marker_icon, 
                                       text_color="white",
                                       font=("Inter", 12, "bold"))

            info_text = (
                f"{marker['name']}:\n"
                f"Lat: {marker['latitude']:.5f}, Lon: {marker['longitude']:.5f}\n\n"
                )
            all_info_text += info_text
        '''
        '''
        # Create an info box frame inside the map widget
        info_frame = Frame(self.map_widget, bg="lightgray")
        info_frame.place(relx=0.99, rely=0.01, anchor="ne", width=260, height=240)
        
        # Create a scrollbar for the info frame
        scrollbar = Scrollbar(info_frame)
        scrollbar.pack(side=RIGHT, fill=Y)
        
        # Create a text widget that will serve as the info box
        info_text_widget = Text(
            info_frame, 
            wrap="word", 
            yscrollcommand=scrollbar.set, 
            bg="#c5dfdf", 
            fg="black", 
            font=("Inter", 12), 
            relief="flat",  # No border
            padx=10  # Add padding to the left
        )
        info_text_widget.pack(expand=True, fill=BOTH)
        
        # Insert the content into the text widget
        info_text_widget.insert("1.0", all_info_text)
        
        # Configure the scrollbar to work with the text widget
        scrollbar.config(command=info_text_widget.yview)
        
        # Disable editing of the text widget to make it behave like a read-only label
        info_text_widget.config(state="disabled")
        
    
    def draw_current_metrics_box(self):
        bgset = '#ffffff'
        bgset1 = '#d31638'
        fgset = 'black'
        fgset1 = 'white'
        
        self.weather_summary.generate_current_weather_summary()
        summary = self.weather_summary.current_weather_summary
        
        current_txt_frame = Frame(self.parent_frame, bg=bgset1)
        current_txt_frame.place(x=1030, y=20, width=280, height=40)
        
        current_txt = Label(current_txt_frame, text='Current Weather', 
                            fg=fgset1, bg=bgset1, font=("Inter", 16, "bold"))
        current_txt.place(x=50, y=1)
    
        self.current_frame = Frame(self.parent_frame, bg=bgset, 
                                   highlightbackground=bgset1,   
                                   highlightthickness=2)
        self.current_frame.place(x=1030, y=50, width=280, height=140)
        
        description = Label(self.current_frame, text=summary['weather_description'], 
                            fg=fgset, bg=bgset, font=("Inter", 16, "bold"))
        description.place(x=90, y=15)
        
        if summary.get('icon_url') == '-': 
            image_data = Image.open('images/remove.png')  
            resized_image = image_data.resize((30, 30), Image.LANCZOS)
            self.emoji_tk = ImageTk.PhotoImage(resized_image)
        else:
            response = requests.get(summary['icon_url'])
            
            if response.status_code == 200:
                image_data = Image.open(BytesIO(response.content))
                # Resize the image (for example, to 60x60 pixels)
                resized_image = image_data.resize((65, 65), Image.LANCZOS)
                # Convert to a format Tkinter can use
                self.emoji_tk = ImageTk.PhotoImage(resized_image)
            else:
                image_data = Image.open('images/remove.png')  
                resized_image = image_data.resize((30, 30), Image.LANCZOS)
                self.emoji_tk = ImageTk.PhotoImage(resized_image)
        
        # Adding the image to the Label in tkinter
        desc_img = Label(self.current_frame, image=self.emoji_tk, bg=bgset)
        desc_img.image = self.emoji_tk  # Prevent garbage collection
        desc_img.place(x=10, y=0)
        
        
        Temp_txt = Label(self.current_frame, text='Temperature', 
                         fg=fgset, bg=bgset, font=("Inter", 12, "bold"))
        Temp_txt.place(x=25, y=55)
        
        Temp = Label(self.current_frame, text=f"{summary['temp']}°C", 
                     fg=fgset, bg=bgset, font=("Inter", 14, "bold"))
        Temp.place(x=170, y=55)
        
        
        Humd_txt = Label(self.current_frame, text='Humidity', 
                         fg=fgset, bg=bgset, font=("Inter", 12, "bold"))
        Humd_txt.place(x=25, y=80)
        
        Humd = Label(self.current_frame, text=f"{summary['humidity']:.1f}%", 
                     fg=fgset, bg=bgset, font=("Inter", 14, "bold"))
        Humd.place(x=170, y=80)
        
        Wind_txt = Label(self.current_frame, text='Wind Speed', 
                         fg=fgset, bg=bgset, font=("Inter", 12, "bold"))
        Wind_txt.place(x=25, y=105)
        
        Wind = Label(self.current_frame, text=f"{summary['wind_speed']}m/s", 
                     fg=fgset, bg=bgset, font=("Inter", 14, "bold"))
        Wind.place(x=170, y=105)
        
        
   
    def draw_summary_metrics_box(self, summary, bgset1, locy, ftitle):
        bgset = '#ffffff'
        fgset = 'balck'
        fgset1 = 'white'
    
        past_txt_frame = Frame(self.parent_frame, bg=bgset1)
        past_txt_frame.place(x=1030, y=locy, width=280, height=40)
        
        past_txt = Label(past_txt_frame, text=ftitle, 
                         fg=fgset1, bg=bgset1, font=("Inter", 16, "bold"))
        past_txt.place(x=35, y=5)
        
        # body frame 2
        self.summary_frame = Frame(self.parent_frame, bg=bgset,
                                     highlightbackground=bgset1,   
                                     highlightthickness=2)
        self.summary_frame.place(x=1030, y=locy+40, width=280, height=200)
    
        # Body Frame 2 Content
        title1 = Label(self.summary_frame, text='Temperature', bg=bgset, font=("Inter", 12, "bold"))
        title1.place(x=5, y=5)


        Thigh = Label(self.summary_frame, text=f"{summary['highest_temp']:.1f}", 
                      bg=bgset, fg='#690571' , font=("Inter", 14, "bold"), anchor='e')
        Thigh.place(x=5, y=25, width=50)
        Thighunit = Label(self.summary_frame, text='°C', bg=bgset, font=("Inter", 10, "bold"))
        Thighunit.place(x=52, y=29)
        Thigh_txt = Label(self.summary_frame, text='Highest', bg=bgset, font=("Inter", 10))
        Thigh_txt.place(x=15, y=50)
        
        Tmean = Label(self.summary_frame, text=f"{summary['avg_temp']:.1f}", 
                      fg='#4c8b3c', bg=bgset, font=("", 14, "bold"), anchor='e')
        Tmean.place(x=95, y=25, width=50)
        Tmeanunit = Label(self.summary_frame, text='°C', bg=bgset, font=("Inter", 10, "bold"))
        Tmeanunit.place(x=142, y=29)
        Tmean_txt = Label(self.summary_frame, text='Average', bg=bgset, font=("Inter", 10))
        Tmean_txt.place(x=105, y=50)
        
        Tlow = Label(self.summary_frame, text=f"{summary['lowest_temp']:.1f}", 
                     fg='#1e75b3', bg=bgset, font=("", 14, "bold"), anchor='e')
        Tlow.place(x=185, y=25, width=50)
        Tlowunit = Label(self.summary_frame, text='°C', bg=bgset, font=("Inter", 10, "bold"))
        Tlowunit.place(x=232 ,y=29)
        Tlow_txt = Label(self.summary_frame, text='Lowest', bg=bgset, font=("Inter", 10))
        Tlow_txt.place(x=195, y=50)
        
        
        title2 = Label(self.summary_frame, text='Humidity', bg=bgset, font=("Inter", 12, "bold"))
        title2.place(x=5, y=70)
        
        Hhigh = Label(self.summary_frame, text=f"{summary['highest_humidity']:.1f}", 
                      bg=bgset, fg='#690571', font=("Inter", 14, "bold"), anchor='e')
        Hhigh.place(x=5, y=90, width=50)
        Hhighunit = Label(self.summary_frame, text='%', bg=bgset, font=("Inter", 10, "bold"))
        Hhighunit.place(x=52, y=94)
        Hhigh_txt = Label(self.summary_frame, text='Highest', bg=bgset, font=("Inter", 10))
        Hhigh_txt.place(x=15, y=115)
        
        
        Hmean = Label(self.summary_frame, text=f"{summary['avg_humidity']:.1f}", 
                      bg=bgset, fg='#4c8b3c', font=("Inter", 14, "bold"), anchor='e')
        Hmean.place(x=95, y=90, width=50)
        Hmeanunit = Label(self.summary_frame, text='%', bg=bgset, font=("Inter", 10, "bold"))
        Hmeanunit.place(x=142, y=94)
        Hmean_txt = Label(self.summary_frame, text='Average', bg=bgset, font=("Inter", 10))
        Hmean_txt.place(x=105, y=115)
        
        Hlow = Label(self.summary_frame, text=f"{summary['lowest_humidity']:.1f}", 
                     bg=bgset, fg='#1e75b3', font=("Inter", 14, "bold"), anchor='e')
        Hlow.place(x=185, y=90, width=50)
        Hlowunit = Label(self.summary_frame, text='%', bg=bgset, font=("Inter", 10, "bold"))
        Hlowunit.place(x=232, y=94)
        Hlow_txt = Label(self.summary_frame, text='Lowest', bg=bgset, font=("Inter", 10))
        Hlow_txt.place(x=195, y=115)
        
        
        title3 = Label(self.summary_frame, text='Average Wind Speed', bg=bgset, font=("Inter", 12, "bold"))
        title3.place(x=5, y=140)
        
        Windvalue = Label(self.summary_frame, text=f"{summary['avg_wind_speed']:.2f}", 
                          bg=bgset, font=("Inter", 14, "bold"), anchor='e')
        Windvalue.place(x=185, y=140, width=50)
        Windunit = Label(self.summary_frame, text='m/s', bg=bgset, font=("Inter", 10, "bold"))
        Windunit.place(x=232, y=144)

        
        title3 = Label(self.summary_frame, text='Total Precipitation', bg=bgset, font=("Inter", 12, "bold"))
        title3.place(x=5, y=165)
        
        Rainvalue = Label(self.summary_frame, text=f"{summary['total_rainfall']:.2f}", 
                          bg=bgset, font=("Inter", 14, "bold"), anchor='e')
        Rainvalue.place(x=185, y=165, width=50)
        Rainunit = Label(self.summary_frame, text='mm', bg=bgset, font=("Inter", 10, "bold"))
        Rainunit.place(x=232, y=169)
        
        
    def save_historical_data(self):
        
        bgset1='#bd9e84'
        
        self.bodyFrame5 = Frame(self.parent_frame, bg='white', 
                                highlightbackground=bgset1,   
                                highlightthickness=2)
        self.bodyFrame5.place(x=1030, y=700, width=280, height=160)
        
        self.startdatelabel = Label(self.bodyFrame5, text="Start Date", bg='white', font=("Inter", 12, "bold"))
        self.startdatelabel.place(x=40, y=20)
        
        self.startdateentry = Entry(self.bodyFrame5, state='readonly', font=("Inter", 12, "bold"))
        self.startdateentry.place(x=130, y=20, width=100)
        # Bind mouse click event to open calendar for start date
        self.startdateentry.bind("<Button-1>", lambda event: self.popup_calendar(self.startdateentry))
        
        
        self.enddatelabel = Label(self.bodyFrame5, text="End Date", bg='white', font=("Inter", 12, "bold"))
        self.enddatelabel.place(x=40, y=60)
        
        self.enddateentry = Entry(self.bodyFrame5, state='readonly', font=("Inter", 12, "bold"))
        self.enddateentry.place(x=130, y=60, width=100)
        # Bind mouse click event to open calendar for end date
        self.enddateentry.bind("<Button-1>", lambda event: self.popup_calendar(self.enddateentry))
        
        
        self.save_data_btn = Button(self.bodyFrame5, text="Save Data for Selected Period", 
                                    bg=bgset1, fg='white', font=("Inter", 12, "bold"),
                                    command=self.process_saving_data)
        self.save_data_btn.place(x=17, y=100)
        
    
    def process_saving_data(self):
        start_date_str = self.startdateentry.get()
        end_date_str = self.enddateentry.get()
        
        if not start_date_str or not end_date_str: 
            messagebox.showerror("Missing Date", "Please fill in both the start and end dates.")
            return
        
        start_date = datetime.strptime(start_date_str, '%Y-%m-%d')
        end_date = datetime.strptime(end_date_str, '%Y-%m-%d')

        # Check if the start date is greater than the end date
        if start_date > end_date:
            messagebox.showerror("Invalid Date Range", "Start date cannot be greater than end date.")
            return
        else:
            if end_date > datetime.now(): 
                messagebox.showerror("Invalid End Date", "End date cannot be greater than the current date.") 
                return
            else:
                location = Point(self.center_lat, self.center_lon)
                daily_data = Daily(location, start=start_date, end=end_date)
                daily_data = daily_data.fetch()
                
                # Reset index to add the date as the first column
                daily_data.reset_index(inplace=True)
                
                # Ask user to select a folder to save the CSV
                folder_selected = filedialog.askdirectory()
                if folder_selected:
                    # Define the file path and save the data as CSV
                    file_path = f"{folder_selected}/{start_date_str}_to_{end_date_str}.csv"
                    daily_data.to_csv(file_path, index=False)
                    messagebox.showinfo("Success", f"Data saved successfully at {file_path}")


    def popup_calendar(self, entry_field):
        # Check if there is already an existing calendar window
        if hasattr(self, 'cal_window') and self.cal_window.winfo_exists():
            return
    
        def set_date():
            selected_date = cal.get_date()
            entry_field.config(state='normal')
            entry_field.delete(0, END)
            entry_field.insert(0, selected_date)
            entry_field.config(state='readonly')
            close_calendar()
    
        def cancel():
            close_calendar()
    
        def close_calendar():
            if hasattr(self, 'cal_window') and self.cal_window.winfo_exists():
                self.cal_window.destroy()
                self.parent_frame.focus_force()  # Force focus back to the main app
    
        def on_focus_out(event=None):
            # Safeguard to close calendar if focus is lost
            close_calendar()
    
        # Create a calendar window
        self.cal_window = Toplevel(self.parent_frame, bg="#f9f4ed")
        self.cal_window.overrideredirect(True)
        self.cal_window.resizable(False, False)
        self.cal_window.grab_set()
    
        # Calculate calendar position near entry
        x = entry_field.winfo_rootx() - 122
        y = entry_field.winfo_rooty() - 240
    
        # Keep calendar within screen bounds
        screen_width = self.cal_window.winfo_screenwidth()
        screen_height = self.cal_window.winfo_screenheight()
        x = max(0, min(x, screen_width - 260))
        y = max(0, min(y, screen_height - 240))
    
        self.cal_window.geometry(f"260x240+{x}+{y}")
    
        # Add calendar widget
        cal = Calendar(
            self.cal_window, selectmode='day',
            date_pattern='yyyy-mm-dd',
            showweeknumbers=False,
            selectbackground="#d35b50",
            weekendbackground="#a1c14b",
            font=("Inter", 11, "bold")
        )
        cal.pack()
    
        # Add buttons for date selection
        Button(self.cal_window, text="Confirm", font=("Inter", 11, "bold"),
               bg='#bd9e84', fg='white', command=set_date).place(x=40, y=200)
    
        Button(self.cal_window, text="Cancel", font=("Inter", 11, "bold"),
               bg='#bd9e84', fg='white', command=cancel).place(x=170, y=200)
    
        # Ensure focus management
        self.cal_window.bind("<FocusOut>", on_focus_out)

