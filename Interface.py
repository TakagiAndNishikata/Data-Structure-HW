import tkinter as tk
import subprocess as sp

main = tk.Tk()
main.title("會A+吧 我都來了")

compress = tk.Label(text = "若要壓縮檔案，請在此欄位輸入欲壓縮的檔案名：")
compress.config(font = ("新細明體",15))
compress.pack()

compressE = tk.Entry(width = 48,font = ("新細明體",15))
compressE.pack()

def runCompress():
    inputfile = compressE.get()
    sp.run(["B093040003-HW8.exe","huffman","-c","-i",inputfile,"-o","Compress-"+inputfile])

butCom = tk.Button(text = "壓縮",font = ("新細明體",10), command = runCompress)
butCom.pack()

Uncompress = tk.Label(text = "若要解壓縮檔案，請在此欄位輸入欲解壓縮的檔案名：")
Uncompress.config(font = ("新細明體",15))
Uncompress.pack()

UncompressE = tk.Entry(width = 48,font = ("新細明體",15))
UncompressE.pack()

def runUncompress():
    inputfile = UncompressE.get()
    sp.run(["B093040003-HW8.exe","huffman","-u","-i",inputfile,"-o","Unompress-"+inputfile])

butUncom = tk.Button(text = "壓縮",font = ("新細明體",10), command = runUncompress)
butUncom.pack()

main.mainloop()