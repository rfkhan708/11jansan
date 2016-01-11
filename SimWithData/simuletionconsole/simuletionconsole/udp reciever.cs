
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Structure;


namespace simuletionconsole
{
    public class UDP_Reciever
    {
        private static readonly UDP_Reciever instance = new UDP_Reciever();
        public static UDP_Reciever Instance
        {
            get  {  return instance;  }
        }
        private int _countdata = 0;
        private int _countolddatadata = 0;
        private CancellationTokenSource cts = new CancellationTokenSource();
        private List<int> _iSubscribe = new List<int>();
        internal event EventHandler<ReadOnlyEventArgs<FinalPrice>> OnDataArrived;
        int BufferSize = 16;
      
        ~UDP_Reciever()
        {
            _iSubscribe.Clear();
            cts.Cancel();
        } 

        internal Int32 Subscribe
        {         
            set  {
                if (!_iSubscribe.Contains(value)) { _iSubscribe.Add(value); }
               }
        }

        System.Timers.Timer timerforchecklogin;
        private void timers_datacheck() {
            timerforchecklogin = new System.Timers.Timer();
            timerforchecklogin.Interval = 30000;
            timerforchecklogin.Start();   }

        internal void UDPReciever (String LanIP = "127.0.0.1", string McastIp = "226.1.1.1" /* "239.70.70.21"*/, int port = 5050)  //10821)   //  internal void UDPReciever(String LanIP = "127.0.0.1", string McastIp = "233.1.2.5", int port = 34330)
        {
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            s.ExclusiveAddressUse = false;
            s.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, Convert.ToInt32(port));
            s.Bind(ipep);
            IPAddress ip = IPAddress.Parse(McastIp);
            s.SetSocketOption(SocketOptionLevel.IP , SocketOptionName.AddMembership, new MulticastOption(ip, IPAddress.Parse("127.0.0.1")));

            byte[] r_req = new byte[BufferSize];
            try
            {
                //Task.Run(() =>
                Task.Factory.StartNew(() =>
                {
                    while (!cts.IsCancellationRequested)
                    {
                        int size = s.Receive(r_req);
                        if (size > 0)
                        {
                            FinalPrice _obj = (FinalPrice)DataPacket.RawDeserialize(r_req, typeof(FinalPrice));
                            //Console.WriteLine("Token {0} Bid {1} Ask {2} LTP {3}",_obj.Token,_obj.MAXBID,_obj.MINASK,_obj.LTP);

                            if (_iSubscribe.Contains(_obj.Token))
                            {
                                OnDataArrived.Raise(OnDataArrived, OnDataArrived.CreateReadOnlyArgs(_obj));
                            }
                        }
                        cts.Token.ThrowIfCancellationRequested();
                    }
                }, cts.Token);

            }
            catch (OperationCanceledException)
            {
                Console.WriteLine("Cancellation invoked");
            }
            catch (AggregateException e)
            {
                if (e.InnerException is OperationCanceledException)
                {

                    if (s != null)
                        if (s.Connected)
                        {
                            s.Shutdown(SocketShutdown.Both);
                            s.Close();
                        }

                }
                else
                {
                    Console.WriteLine("Some unexpected exception ");
                }
            }
            catch (Exception Ex)
            {
                Console.WriteLine("Exception Raised " + Ex.Message);
            }


        }
    }
}
