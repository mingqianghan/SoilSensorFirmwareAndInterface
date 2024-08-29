# -*- coding: utf-8 -*-
"""
Created on Sun Apr 21 21:36:42 2024

@author: mingqiang
"""
import serial.tools.list_ports
import time
import tkinter as tk

class SerialCtrl():
    def __init__(self):
        self.sync_cnt = 10
    
    def getCOMList(self):
        ports = serial.tools.list_ports.comports()
        self.com_list = [com[0] for com in ports]
        self.com_list.insert(0, "-")
        
    def SerialOpen(self, gui):
        try:
            self.ser.is_open 
        except:
            PORT = gui.selected_com.get()
            BAUD = gui.selected_bd.get()
            self.ser = serial.Serial()
            self.ser.baudrate = BAUD
            self.ser.port = PORT
            self.ser.timeout = 0.1
        
        try:
            if self.ser.is_open:
                self.ser.status = True
            else:
                PORT = gui.selected_com.get()
                BAUD = gui.selected_bd.get()
                self.ser = serial.Serial()
                self.ser.baudrate = BAUD
                self.ser.port = PORT
                self.ser.timeout = 0.1
                self.ser.open()
                self.ser.status =True
        except:
            self.ser.status = False
                    
    def SerialClose(self):
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False
      
    def Close_Com(self):
        if hasattr(self, 'ser'):
            self.ser.close()
            
    def FrequencyID_UART_TX(self, gui): 
        for i in range(0, len(gui.data.fre_IDs), 3):
            chunk = gui.data.fre_IDs[i:i + 3]
            # print(chunk)
            # print(chunk)
            chartouart = f"#C#{len(chunk)}#"
            # j+=len(chunk)
            # print(j)
            for IDs in chunk:
                highByte = (IDs[0] >> 8) & 0xFF   # Extract the high byte
                lowByte = IDs[0] & 0xFF           # Extract the low byte
                chartouart += f"{highByte:d}{lowByte:03d}"
                #print(f"{highByte}:{chr(highByte)} {lowByte}:{chr(lowByte)}")
            # print(chartouart)
            chartouart += "\n"
            self.ser.write(chartouart.encode())
            # print(chartouart)
            
        
    def SerialSync(self, gui):
        self.threading = True
        cnt = 0
        while self.threading:
            try:
                self.ser.write(gui.data.sync_out.encode())
                gui.conn.sync_status["text"] = "..Sync.."
                gui.conn.sync_status["fg"] = "orange"
                gui.data.RowMsg = self.ser.readline()
                
                if gui.data.RowMsg: 
                    #print(f"RowMsg: {gui.data.RowMsg}") 
                    gui.data.DecodeMsg()
                    if gui.data.sync_ok:
                        gui.conn.Load_fre["state"] = "active"
                        
                        gui.conn.sync_status["text"] = "OK"
                        gui.conn.sync_status["fg"] = "green"
                     
                        gui.conn.allnode_status["text"] = \
                                    ', '.join(map(str, gui.data.sensorIDs))
                        gui.conn.allnode_status["fg"] = "green"
                       
                        gui.fieldInfo.sitename_dis["text"] = gui.data.SiteName
                        gui.fieldInfo.sitename_dis["fg"] = "green"
                        
                        gui.fieldInfo.Latitude_dis["text"] = \
                                              f"{gui.data.SiteLatitude:.6f}"
                        gui.fieldInfo.Latitude_dis["fg"] = "green"
                        
                        gui.fieldInfo.Longitude_dis["text"] = \
                                             f"{gui.data.SiteLongitude:.6f}"
                        gui.fieldInfo.Longitude_dis["fg"] = "green"
                        
                        gui.fieldInfo.sensor_num_dis["text"] = \
                                             f"{len(gui.data.sensorIDs)}"
                        gui.fieldInfo.sensor_num_dis["fg"] = "green"
                        
                        self.threading = False
                        break
                    
                if self.threading == False: break
            except Exception as e:
                print(e)
            cnt += 1
            if self.threading == False: break
            if cnt > self.sync_cnt:
                cnt = 0
                gui.conn.sync_status["text"] = "failed"
                gui.conn.sync_status["fg"] = "red"
                time.sleep(0.5)
                if self.threading == False: break
            
                
    def SerialStream(self, gui):
        self.threading = True
        gui.data.sensordnIDs = []
        gui.nodedone_status["text"] = "..."
        gui.nodedone_status["fg"] = "orange"
        
        # TXFreIDs, MXStart TimeRqst, RandomRqst, DataRec, WaitforMCU, MXStop
        self.SerState = "TXFreIDs"
        
        
        while self.threading:
            
            if not self.threading: break
        
            if self.SerState == "TXFreIDs":
                FreHead = f"#F#{len(gui.data.fre_IDs):04d}\n"
                self.ser.write(FreHead.encode())
                self.FrequencyID_UART_TX(gui)
                FreTail = "#P#\n"
                self.ser.write(FreTail.encode())
            elif self.SerState  == "MXStart":
                self.ser.write(gui.data.Start_out.encode())
                gui.data.radio_test_data = []
                gui.data.saved_data = []
                gui.data.data_error_count = 0
            elif self.SerState == "TimeRqst":
                gui.data.TimeGeneration()
                self.ser.write(gui.data.TimeRes.encode())
            elif self.SerState == "RandomRqst":
                gui.data.RandomNumberGeneration()
                self.ser.write(gui.data.random_radio.encode())
            elif self.SerState == "DataRec":
                gui.data.SaveRadioData()
            elif self.SerState == "MXStop":
                gui.text_dis["text"] = "Node Status"
                gui.Node_status["text"] = "All Done, Load Fre. IDs to recollect."
                gui.Node_status["fg"] = "green"
                
                gui.Load_fre["state"] = "active"
                gui.load_status["text"] = "Pending..."
                gui.load_status["fg"] = "orange"
                gui.btn_start_collection["state"] = "disabled"
                gui.btn_stop_collection["state"] = "disabled"
                
                self.threading = False
            elif self.SerState == "WaitforMCU":
                pass
            
            if self.threading:
                if self.ser.is_open: 
                    while self.ser.in_waiting == 0: 
                        if not self.threading: break
                        time.sleep(0.01)
                        if not self.threading: break
                
                if not self.threading: break 
            
                gui.data.RowMsg = self.ser.readline()
                #print(f"RowMsg: {gui.data.RowMsg}") 
                gui.logger.logger.insert(tk.END, gui.data.RowMsg)
                if gui.logger.auto_scroll.get():
                    gui.logger.logger.see(tk.END)
                
                gui.data.DecodeMsg()
            
                if gui.data.fre_rec:
                    self.SerState = "MXStart"
                if gui.data.t_rqst_ok:
                    gui.data.radio_cmd = int(gui.data.msg[2])
                    self.SerState = "TimeRqst"
                if gui.data.random_ok:
                    gui.data.random_num_len = int(gui.data.msg[1])
                    gui.data.radio_cmd = int(gui.data.msg[2])
                    self.SerState = "RandomRqst"
                if gui.data.St_tlk_ok:
                    gui.text_dis["text"] = f"Node {gui.data.current_node} Status"
                    gui.Node_status["text"] = "Pending..."
                    gui.Node_status["fg"] = "orange"
                    gui.ACK_status["text"] = "Pending..."
                    gui.ACK_status["fg"] = "orange"
                    gui.time_cal_status["text"] = "Pending..."
                    gui.time_cal_status["fg"] = "orange"
                    gui.freID_status["text"] = "Pending..."
                    gui.freID_status["fg"] = "orange"
                    gui.data_rec_status["text"] = "Pending..."
                    gui.data_rec_status["fg"] = "orange"
                    gui.data_save_status["text"] = "Pending..."
                    gui.data_save_status["fg"] = "orange"
                    self.SerState = "WaitforMCU"
                if gui.data.Info_display:
                    self.SerState = "WaitforMCU"
                    if 'FreLenNotMactched' in gui.data.msg[1]:
                        gui.load_status["text"] = "Error UART TX"
                        gui.load_status["fg"] = "red"
                        gui.Load_fre["state"] = "active"
                        gui.btn_start_collection["state"] = "disabled"
                        gui.btn_stop_collection["state"] = "disabled"
                        self.threading = False
                    if(len(gui.data.msg)>2):
                        if 'Waking up' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Waking up the sensor..."
                            gui.Node_status["fg"] = "green"
                        elif 'Collecting data' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Collecting data..."
                            gui.Node_status["fg"] = "green"
                        elif 'CMD timeout' in gui.data.msg[2]:
                            gui.Node_status["text"] = "CMD wait timeout."
                            gui.Node_status["fg"] = "red"
                        elif 'DATA timeout' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Data wait timeout."
                            gui.Node_status["fg"] = "red"
                        elif 'TimeCalFailed' in gui.data.msg[2]:
                            gui.time_cal_status["text"] = \
                                f"{gui.data.msg[3]} times failed"
                            gui.time_cal_status["fg"] = "red" 
                            gui.Node_status["text"] = "Time calibration failed"
                            gui.Node_status["fg"] = "red"
                        elif 'ResendFailed' in gui.data.msg[2]:
                            gui.data_rec_status["text"] = \
                                f"Resend {gui.data.msg[3]} times failed"
                            gui.data_rec_status["fg"] = "red"
                            gui.Node_status["text"] = "Saved data with errors"
                            gui.Node_status["fg"] = "red"
                        elif 'Calibrating time' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Calibrating time..."
                            gui.Node_status["fg"] = "green"
                        elif 'Sending FreIDs' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Sending Fre.IDs to radio..."
                            gui.Node_status["fg"] = "green"
                        elif 'NodeDone' in gui.data.msg[2]:
                            if int(gui.data.msg[1]) in gui.data.sensorIDs: 
                                gui.data.sensordnIDs.append\
                                    (int(gui.data.msg[1])) 
                             
                                if gui.data.sensordnIDs: 
                                    gui.nodedone_status["text"] = \
                                    ', '.join(map(str, gui.data.sensordnIDs))
                                    gui.nodedone_status["fg"] = "green"
                                else:
                                    gui.nodedone_status["text"] = "..."
                                    gui.nodedone_status["fg"] = "orange"
                        elif 'TimeMatched' in gui.data.msg[2]:
                            gui.time_cal_status["text"] = "Yes"
                            gui.time_cal_status["fg"] = "green" 
                            gui.data.radio_test_data.append(gui.data.time_to_Radio)
                        elif 'FreIDsTXOK' in gui.data.msg[2]:
                            gui.freID_status["text"] = "UG nodes received OK"
                            gui.freID_status["fg"] = "green"
                        elif'FreIDsFailed' in gui.data.msg[2]:
                            gui.freID_status["text"] = \
                                f"{gui.data.msg[3]} times failed"
                            gui.freID_status["fg"] = "orange"
                            gui.Node_status["text"] = "Fre IDs. TX failed"
                            gui.Node_status["fg"] = "red"
                        elif 'NodeStop' in gui.data.msg[2]:
                            self.SerState = "MXStart"
                        elif 'AllDone' in gui.data.msg[2]:
                            self.SerState = "MXStop"
                        elif 'AKReceived' in gui.data.msg[2]:
                            gui.ACK_status["text"] = "Yes"
                            gui.ACK_status["fg"] = "green"
                         
                if gui.data.Received_data:
                    if(len(gui.data.msg)>2):
                        if 'Recdata' in gui.data.msg[2]:
                            gui.data.Data_len = int(gui.data.msg[3])
                            gui.data_rec_status["text"] = "Yes"
                            gui.data_rec_status["fg"] = "green"
                            self.SerState = "WaitforMCU"
                        elif 'Savedata' in gui.data.msg[2]:
                            gui.Node_status["text"] = "Saving received data..."
                            gui.Node_status["fg"] = "green"
                            self.SerState = "DataRec"
                        elif 'DataNoErrors' in gui.data.msg[2]:
                            gui.data_rec_status["text"] = "Yes, no errors."
                            gui.data_rec_status["fg"] = "green"
                            gui.Node_status["text"] = "Saving to file."
                            gui.Node_status["fg"] = "green"
                            gui.data.RadioDataToFile()
                            # make sure saving data is successful
                            if gui.data.data_save_ok: 
                                gui.data_save_status["text"] = "Yes"
                                gui.data_save_status["fg"] = "green"
                                gui.Node_status["text"] = "Data saved to file."
                                gui.Node_status["fg"] = "green"
                            else: 
                                # print("Error saving file")
                                gui.Node_status["text"] = "Error saving file."
                                gui.Node_status["fg"] = "red"
                            self.SerState = "WaitforMCU"
                        elif 'DataWithErrors' in gui.data.msg[2]:
                            gui.data_rec_status["text"] = "Yes,with errors"
                            gui.data_rec_status["fg"] = "red"
                            gui.Node_status["text"] = "Requesting to resend..."
                            gui.Node_status["fg"] = "green"
                            
                            gui.data.data_error_count+=1
                            gui.data.RadioDataToFile()
                            # make sure saving data is successful
                            if gui.data.data_save_ok: 
                                gui.Node_status["text"] = "Error data saved to file."
                                gui.Node_status["fg"] = "green" 
                                 
                            else: 
                                # print("Error saving file")
                                gui.Node_status["text"] = "Error data saving failed."
                                gui.Node_status["fg"] = "red"
                            self.SerState = "WaitforMCU"
                        elif 'WrongLength' in gui.data.msg[2]:
                            # print(f"Recived:{gui.data.Data_len} Expected:{gui.data.msg[3]}")
                            gui.data_rec_status["text"] = "Yes,wrong length"
                            gui.data_rec_status["fg"] = "red"
                            gui.Node_status["text"] = "Requesting to resend..."
                            gui.Node_status["fg"] = "green"
                            
                            gui.data.data_error_count+=1
                            gui.data.RadioDataToFile()
                            # make sure saving data is successful
                            if gui.data.data_save_ok: 
                                gui.Node_status["text"] = "Error data saved to file."
                                gui.Node_status["fg"] = "green" 
                                 
                            else: 
                                # print("Error saving file")
                                gui.Node_status["text"] = "Error data saving failed."
                                gui.Node_status["fg"] = "red"
                            self.SerState = "WaitforMCU"
                                
                                
if __name__ == "__main__":
    SerialCtrl()
