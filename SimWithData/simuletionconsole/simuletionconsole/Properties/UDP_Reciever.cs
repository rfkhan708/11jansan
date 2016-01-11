using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Structure;
//using ZeroMQ;
using NNanomsg.Protocols;


namespace simuletionconsole
{
	class UDP_RecieverData
	{


		private static readonly UDP_RecieverData instance = new UDP_RecieverData("tcp://" +"192.168.168.36"+ ":" +7070);
		public static UDP_RecieverData Instance
		{
			get
			{
				return instance;
			}
		}
		public CancellationTokenSource cts = new CancellationTokenSource();
		private List<int> _iSubscribe = new List<int>();
		struct sendData
		{
			public int Token, Bid1, Ask1, LTP;

		}

		internal event EventHandler<ReadOnlyEventArgs<string>> OnDataStatusChange;
		private int _countdata = 0;
		private int _countolddatadata = 0;

		SubscribeSocket subscriber = null;
		private string DATAAddress;
		internal ConcurrentDictionary<int,FinalPrice> TokenData = new ConcurrentDictionary<int, FinalPrice> ();
		public UDP_RecieverData(string DataAddress)
		{
			DATAAddress = DataAddress;
			subscriber = new SubscribeSocket();
			subscriber.Options.ReconnectInterval = new TimeSpan(0, 0, 1);

			subscriber.Connect(DataAddress);
			Console.WriteLine("NANOMQ UDP_Reciever Start DataAddress: " + DataAddress);
		}
		System.Timers.Timer timerforchecklogin;
		private void timers_datacheck()
		{
			timerforchecklogin = new System.Timers.Timer();
			timerforchecklogin.Interval = 30000;
			timerforchecklogin.Start();
			//  timerforchecklogin.Elapsed += timerforchecklogin_Elapsed;
		}
		void timerforchecklogin_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
		{
			try
			{
				if (_countolddatadata >= _countdata)
				{
					this.OnDataStatusChange.Raise(OnDataStatusChange, OnDataStatusChange.CreateReadOnlyArgs("STOP"));

				}
				else
				{
					this.OnDataStatusChange.Raise(OnDataStatusChange, OnDataStatusChange.CreateReadOnlyArgs("START"));
					_countolddatadata = _countdata;
				}
			}
			catch (Exception ex)
			{

				Console.WriteLine(ex.Message);
			}
		}


		~UDP_RecieverData()
		{

			_iSubscribe.Clear();
			cts.Cancel();
			//subscriber.Disconnect (DATAAddress);
			if (subscriber != null)
			{
				subscriber.Dispose();
			}

		}
		internal Int32 Subscribe
		{
			//	get { return mSeqNumber; }

			set
			{
				if (!_iSubscribe.Contains(value))
				{
					_iSubscribe.Add(value);
					if (subscriber != null)
						subscriber.Subscribe(BitConverter.GetBytes(value));
				}

			}
		}



		internal Int32 UnSubscribe
		{
			//	get { return mSeqNumber; }

			set
			{
				if (_iSubscribe.Contains(value))
				{
					_iSubscribe.Remove(value);
					if (subscriber != null)
						subscriber.Unsubscribe(BitConverter.GetBytes(value));
				}

			}
		}



		internal void UDPReciever()
		{

			try
			{
				timerforchecklogin = new System.Timers.Timer();
				timerforchecklogin.Interval = 30000;
				timerforchecklogin.Start();
				timerforchecklogin.Elapsed += timerforchecklogin_Elapsed;
				Task.Factory.StartNew(() =>
					{

						while (true)
						{

							//		string address = subscriber.Receive (Encoding.Unicode);
							//	    byte[] buffer = new byte[512];
							//	    int bufferSize = subscriber.Receive (buffer);

							var buffer = subscriber.Receive();
							if (buffer == null)
							{
								this.OnDataStatusChange.Raise(OnDataStatusChange, OnDataStatusChange.CreateReadOnlyArgs("STOP"));
								continue;
							}
							FinalPrice _obj = (FinalPrice)DataPacket.RawDeserialize(buffer.Skip(4).Take(buffer.Length - 4).ToArray(), typeof(FinalPrice));

							if (_obj.Token == 111)
							{
								Console.WriteLine(" SomeThing Wrong in DATA Server");
								this.OnDataStatusChange.Raise(OnDataStatusChange, OnDataStatusChange.CreateReadOnlyArgs("START"));
								//	OnDataError.Invoke();
								continue;
							}

							if (_iSubscribe.Contains(_obj.Token))
							{
								UDP_RecieverData.Instance.TokenData.AddOrUpdate(_obj.Token, _obj, (k, v) => _obj);
							
							}
						}
					});
			}
			catch (OperationCanceledException e)
			{
				Console.WriteLine("Cancellation invoked");
			}
			catch (AggregateException e)
			{

				Console.WriteLine("Some unexpected exception ");

			}
			catch (Exception Ex)
			{
				Console.WriteLine("Exception Raised " + Ex.StackTrace);
			}


		}
	}
}