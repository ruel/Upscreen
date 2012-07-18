using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Xml;

namespace Upscreen
{
    public partial class frmMain : Form
    {
        XKeyboardHook hook;

        int hid;
        bool success;
        string apiKey, imgurl, tmpName;

        public frmMain()
        {
            InitializeComponent();
            hook = new XKeyboardHook();
            hook.KeyPressed += new EventHandler<KeyPressedEventArgs>(hook_KeyPressed);

            tmpName = Environment.GetEnvironmentVariable("TEMP") + @"\upscreen.png";
            apiKey = Upscreen.Properties.Settings.Default.AnonAPI;
            success = false;

            registerHotkey();
        }

        private void hook_KeyPressed(object sender, KeyPressedEventArgs e)
        {
            if (Clipboard.ContainsImage())
            {
                Image img = Clipboard.GetImage();
                img.Save(tmpName);
                PostToImgur(tmpName);
            }
        }

        public void PostToImgur(string imagFilePath)
        {
            byte[] imageData;

            FileStream fileStream = File.OpenRead(imagFilePath);
            imageData = new byte[fileStream.Length];
            fileStream.Read(imageData, 0, imageData.Length);
            fileStream.Close();

            const int MAX_URI_LENGTH = 32766;
            string base64img = System.Convert.ToBase64String(imageData);
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < base64img.Length; i += MAX_URI_LENGTH)
            {
                sb.Append(Uri.EscapeDataString(base64img.Substring(i, Math.Min(MAX_URI_LENGTH, base64img.Length - i))));
            }

            string uploadRequestString = "image=" + sb.ToString() + "&key=" + apiKey;

            HttpWebRequest webRequest = (HttpWebRequest)WebRequest.Create("http://api.imgur.com/2/upload");
            webRequest.Method = "POST";
            webRequest.ContentType = "application/x-www-form-urlencoded";
            webRequest.ServicePoint.Expect100Continue = false;

            StreamWriter streamWriter = new StreamWriter(webRequest.GetRequestStream());
            streamWriter.Write(uploadRequestString);
            streamWriter.Close();

            WebResponse response = webRequest.GetResponse();
            Stream responseStream = response.GetResponseStream();
            StreamReader responseReader = new StreamReader(responseStream);

            string responseString = responseReader.ReadToEnd();
            try {
                parseXml(responseString);
            }
            catch (Exception ex)
            {
                success = false;
                nicMain.ShowBalloonTip(1000, "Error", ex.Message, ToolTipIcon.Error);
            }
        }

        private void parseXml(string xml)
        {
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(xml);
            XmlNode node = doc.SelectSingleNode("//upload/links/imgur_page");
            imgurl = node.InnerText;
            success = true;
            nicMain.ShowBalloonTip(10000, "Success", "Click here for the image page", ToolTipIcon.Info);
            File.Delete(tmpName);
        }

        private void registerHotkey()
        {
            // Ctrl (2) + Shift (4) + PrntScrn (0x2C)
            hid = hook.RegisterHotKey(6, 0x2C);
        }

        private void unRegisterHotkey()
        {
            hook.UnRegisterHotkey(hid);
        }

        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            unRegisterHotkey();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void authorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start("http://ruel.me");
        }

        private void editHotkeyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start("https://github.com/ruel/Upscreen");
        }

        private void nicMain_BalloonTipClicked(object sender, EventArgs e)
        {
            if (success) 
            {
                Process.Start(imgurl);
            }
        }
    }
}
