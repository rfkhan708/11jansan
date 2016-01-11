using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Concurrent;
using Structure;
using System.Security.Cryptography;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Net;
using System.IO;


namespace simuletionconsole
{
    class Program
    {
        ConcurrentDictionary<long, MS_OE_RESPONSE_TR> concurrent = new ConcurrentDictionary<long, MS_OE_RESPONSE_TR>();
        //   ConcurrentDictionary<int, tokenval> _contoen = new ConcurrentDictionary<int, tokenval>();
		private tcpServer.TcpServer tcpServer1;
        ConcurrentDictionary<int, ConcurrentDictionary<int, ConcurrentDictionary<long, MS_OE_RESPONSE_TR>>> _concortdict = new ConcurrentDictionary<int, ConcurrentDictionary<int, ConcurrentDictionary<long, MS_OE_RESPONSE_TR>>>();

        //private  static readonly frmMain instance = new frmMain();
        //public static frmMain Instance
        //{
        //    get { return instance; }
        //}
        public delegate void invokeDelegate();

        private int count = 0;
        public Program()
        {
            this.tcpServer1 = new tcpServer.TcpServer();
            btnChangePort_Click(null, null);

            iccount += frmMain_iccount;
			UDP_RecieverData.Instance.UDPReciever();


            //this.tcpServer1.IdleTime = 50;
            //this.tcpServer1.IsOpen = false;
            //this.tcpServer1.MaxCallbackThreads = 100;
            //this.tcpServer1.MaxSendAttempts = 3;
            //this.tcpServer1.Port = -1;
            //this.tcpServer1.VerifyConnectionInterval = 0;
            this.tcpServer1.OnConnect += new tcpServer.tcpServerConnectionChanged(this.tcpServer1_OnConnect);
            this.tcpServer1.OnDataAvailable += new tcpServer.tcpServerConnectionChanged(this.tcpServer1_OnDataAvailable);

        }
        public double DoubleEndianChange(double value)
        {
            return BitConverter.ToDouble(BitConverter.GetBytes(value).Reverse().ToArray(), 0);
        }
        ConcurrentDictionary<int, FinalPrice> _concurrentDataDictionary = new ConcurrentDictionary<int, FinalPrice>();
        FinalPrice _TempFP = new FinalPrice();

        void Instance_OnDataArrived(object sender, ReadOnlyEventArgs<FinalPrice> Stat)
        {
            try
            {
                _concurrentDataDictionary.AddOrUpdate(Stat.Parameter.Token, _TempFP, (k, v) => _TempFP);
                //  _concurrentDataDictionary.GetOrAdd(Stat.Parameter.Token,  _TempFP);
                //   _TempFP = Stat.Parameter;

                var _valldata = _concortdict[Stat.Parameter.Token][1];
                foreach (var item in _valldata)
                {
                    if (item.Value.Price >= Stat.Parameter.MINASK)
                    {
                        tradeconferm_manual(item.Value);
                        Console.WriteLine("Traded");
                    }

                }

            }
            catch (Exception Ex)
            {
                Console.WriteLine(" Exception " + Ex.StackTrace.ToString());
            }
        }


        #region MyRegion
        private void btnChangePort_Click(object sender, EventArgs e)
        {
            try
            {
                openTcpPort();
            }
            catch (FormatException)
            {
                Console.WriteLine("Port must be an integer", "Invalid Port");
            }
            catch (OverflowException)
            {
                Console.WriteLine("Port is too large", "Invalid Port");
            }
        }

        private void openTcpPort()
        {
            tcpServer1.Close();
            Console.WriteLine("Please enter ip address....");

			tcpServer1._ip =Console.ReadLine();
            Console.WriteLine("Please enter port....");
			int prt = 9601;
			if (prt>0)//int.TryParse (Console.ReadLine(),out prt)) 
			{

				tcpServer1.Port = prt;
				Console.WriteLine (tcpServer1._ip+":"+tcpServer1.Port.ToString ());
				tcpServer1.Open ();
				displayTcpServerStatus ();
			} else {
				Console.WriteLine ("Error");
				Console.ReadLine ();
			}
        }

        private void displayTcpServerStatus()
        {
            if (tcpServer1.IsOpen)
            {
                Console.WriteLine("PORT OPEN");

                stpformat();
            }
            else
            {
                Console.WriteLine("PORT NOT OPEN");

            }
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            send();
        }

        public void sendbuffer(byte[] buffer)
        {

            tcpServer1.Send(buffer);

            // logData(true, data);
        }

        private void send()
        {
            string data = "";
            Console.WriteLine("send Receve.." + System.DateTime.Now.ToString());
            //foreach (string line in txtText.Lines)
            //{
            //    data = data + line.Replace("\r", "").Replace("\n", "") + "\r\n";
            //}
            //data = data.Substring(0, data.Length - 2);

            //   tcpServer1.Send(data);

            //  logData(true, data);
        }

        public void logData(bool sent, string text)
        {
            Console.WriteLine("Receve.." + System.DateTime.Now.ToString());
        }


		public  double DateTimeToUnixTimestamp(DateTime dateTime)
		{
			return (TimeZoneInfo.ConvertTimeToUtc(dateTime) - new DateTime(1980, 1, 1)).TotalSeconds;
		}

		public static long ToEpochTime(DateTime dateTime)
		{
			var date = dateTime.ToUniversalTime();
			var ticks = date.Ticks - new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc).Ticks;
			var ts = ticks / TimeSpan.TicksPerSecond;
			return ts;
		}


        private void frmMain_iccount()
        {
            var buff = Logic.Instance.GetPF(DataPacket.RawSerialize(
                      new INVITATION_MESSAGE
                      {
                          TransactionCode = IPAddress.HostToNetworkOrder((short)15000),
                          InvitationCount = IPAddress.HostToNetworkOrder((short)1),
                      })
                    );
            sendbuffer(buff);
        }
        #endregion

        delegate void delegate_sendincount();
        event delegate_sendincount iccount;
        private void tcpServer1_OnDataAvailable(tcpServer.TcpServerConnection connection)
        {
            byte[] data = readStream(connection.Socket);
			//Console.WriteLine ("data Length :"+data.Length);

            var pf = (StrPacketFormate)DataPacket.RawDeserialize(data, typeof(StrPacketFormate));


            if (!verifyMd5Hash(data.Skip(24).Take(data.Length - 24).ToArray(), pf.CheckSum))
                Console.WriteLine("CheckSum Failed");

            if (!verifySeqNo(IPAddress.HostToNetworkOrder(pf.SequenceNumber)))
				Console.WriteLine("SequenceNumber Failed");






            iccount.Invoke();
            if (data != null)
            {
                SIGN_ON_REQUEST_OUT(data);
              //  string dataStr = Encoding.ASCII.GetString(data);
                //invokeDelegate del = () =>
                //{
                //    logData(false, dataStr);
                //};
                //Invoke(del);
                data = null;
            }
        }
        int LastSeqNo;
        public bool verifySeqNo(int SeqNo)
        {
            if (++LastSeqNo == SeqNo)
                return true;
            else
                return true;
        }
        MD5 Md5HashVarify = MD5.Create();
        public bool verifyMd5Hash(byte[] buffer, byte[] CheckSum)
        {
            byte[] input = Md5HashVarify.ComputeHash(buffer);
            string hashOfInput = System.Text.Encoding.UTF8.GetString(input);
            string hash = System.Text.Encoding.UTF8.GetString(CheckSum);
            StringComparer comparer = StringComparer.OrdinalIgnoreCase;
            if (0 == comparer.Compare(hashOfInput, hash))
                return true;
            else
                return false;
        }


        public static byte GetBitsToByteValue(byte bit0, byte bit1, byte bit2, byte bit3, byte bit4, byte bit5, byte bit6, byte bit7)
        {
            #region

            //if (!cynpYinO2Y3VZVnYT8.XIXJKT59x(4))

            #endregion

            var numArray1 = new[]
            {
                bit0,
                bit1,
                bit2,
                bit3,
                bit4,
                bit5,
                bit6,
                bit7
            };
            var numArray2 = numArray1;
            var num = (byte)0;
            for (var index = (byte)0; (int)index < 8; ++index)
                num |= (int)numArray2[index] == 1 ? (byte)(1 << index) : (byte)0;
            return num;
        }


		object _oblock = new object ();
        public void SIGN_ON_REQUEST_OUT(byte[] buffer)  //--2301
        {
			lock(_oblock)
			{
            byte[] buf = { };
            buffer = buffer.Skip(26).ToArray();
            //IPAddress.HostToNetworkOrder (BitConverter.ToInt16 (buffer, 0))

            if (buffer.Length != 40)
            {
                var _checkval = (MS_SYSTEM_INFO_REQ)DataPacket.RawDeserialize(buffer, typeof(MS_SYSTEM_INFO_REQ));



                int trcode = IPAddress.NetworkToHostOrder(_checkval.header_obj.TransactionCode);

                switch (trcode)
                {
                    case 2300:
                        MS_SIGNON obj = (MS_SIGNON)DataPacket.RawDeserialize(buffer, typeof(MS_SIGNON));
                        obj.header_obj.TransactionCode = IPAddress.NetworkToHostOrder((short)2301);
                        obj.header_obj.ErrorCode = IPAddress.NetworkToHostOrder((short)0);
                        obj.BrokerStatus = Convert.ToByte('A');
                        obj.MemberType = IPAddress.NetworkToHostOrder((short)2);
                        obj.ClearingStatus = Convert.ToByte('A');
                        buf = Logic.Instance.GetPF(DataPacket.RawSerialize(obj));
                        sendbuffer(buf);
                        break;
                    case 1600:
                        MS_SYSTEM_INFO_REQ obj1 = (MS_SYSTEM_INFO_REQ)DataPacket.RawDeserialize(buffer, typeof(MS_SYSTEM_INFO_REQ));
                        MS_SYSTEM_INFO_DATA objdata = new MS_SYSTEM_INFO_DATA();
                        objdata.SolicitorPeriod = IPAddress.NetworkToHostOrder((short)0);
                        objdata.header_obj.AlphaChar = obj1.header_obj.AlphaChar;
                        objdata.header_obj.TransactionCode = IPAddress.NetworkToHostOrder((short)1601);
                        objdata.header_obj.TimeStamp1 = obj1.header_obj.TimeStamp1;
                        objdata.header_obj.TimeStamp2 = obj1.header_obj.TimeStamp2;
                        objdata.header_obj.Timestamp = obj1.header_obj.Timestamp;
                        objdata.header_obj.ErrorCode = obj1.header_obj.ErrorCode;
                        objdata.header_obj.LogTime = obj1.header_obj.LogTime;
                        objdata.header_obj.MessageLength = obj1.header_obj.MessageLength;
                        objdata.header_obj.TraderId = obj1.header_obj.TraderId;
                        objdata.st_obj.Normal = IPAddress.NetworkToHostOrder((short)0);
                        objdata.st_ex_obj.Normal = IPAddress.NetworkToHostOrder((short)0);
                        objdata.st_pl_obj.Normal = IPAddress.NetworkToHostOrder((short)0);
                        objdata.UpdatePortfolio = Convert.ToByte('a');
                        objdata.MarketIndex = 0;
                        objdata.DefaultSettlementPeriod_normal = IPAddress.NetworkToHostOrder((short)0);
                        objdata.DefaultSettlementPeriod_spot = IPAddress.NetworkToHostOrder((short)0);
                        objdata.DefaultSettlementPeriod_auction = IPAddress.NetworkToHostOrder((short)0);
                        objdata.CompetitorPeriod = IPAddress.NetworkToHostOrder((short)0);
                        objdata.SolicitorPeriod = IPAddress.NetworkToHostOrder((short)0);
                        objdata.WarningPercent = IPAddress.NetworkToHostOrder((short)0);
                        objdata.VolumeFreezePercent = IPAddress.NetworkToHostOrder((short)0);
                        objdata.SnapQuoteTime = IPAddress.NetworkToHostOrder((short)0);
                        objdata.Reserved1 = IPAddress.NetworkToHostOrder((short)0);
                        objdata.BoardLotQuantity = IPAddress.NetworkToHostOrder(0);
                        objdata.TickSize = IPAddress.NetworkToHostOrder(0);
                        objdata.MaximumGtcDays = IPAddress.NetworkToHostOrder((short)0);
                        objdata.st_stoc_obj.Reserved = Convert.ToByte('r');
                        objdata.st_stoc_obj.Reserved2 = Convert.ToByte('r');
                        objdata.DisclosedQuantityPercentAllowed = IPAddress.NetworkToHostOrder((short)0);
                        objdata.RiskFreeInterestRate = IPAddress.NetworkToHostOrder((short)0);
                        buf = Logic.Instance.GetPF(DataPacket.RawSerialize(objdata));
                        int lth = buf.Length;
                        sendbuffer(buf);
                        break;
				case 20000:
						var obj_1 = (MS_OE_REQUEST_TR)DataPacket.RawDeserialize (buffer, typeof(MS_OE_REQUEST_TR));
					//UDP_Reciever.Instance.Subscribe = IPAddress.NetworkToHostOrder (obj_1.TokenNo);
					Console.WriteLine ("**********************************************************************\n\n\n\n");
					Console.WriteLine (" TransactionCode recieved  \t" + trcode + "\t buffer.Length " + buffer.Length + "\t Buy_SellIndicator = " + IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator) + "\tTokenNo=" + IPAddress.HostToNetworkOrder (obj_1.TokenNo)+" Price :"+IPAddress.HostToNetworkOrder(obj_1.Price));
					Console.WriteLine ("**********************************************************************\n\n\n\n");

					UDP_RecieverData.Instance.Subscribe = IPAddress.HostToNetworkOrder (obj_1.TokenNo);
					var obj_2 = new MS_OE_RESPONSE_TR {
						TransactionCode = IPAddress.NetworkToHostOrder ((short)20073),
						ReasonCode = IPAddress.NetworkToHostOrder ((short)0),
						BookType = IPAddress.NetworkToHostOrder ((short)1),
						GoodTillDate = IPAddress.NetworkToHostOrder (0),
						st_ord_flg_obj = new ST_ORDER_FLAGS {
							STOrderFlagIn = GetBitsToByteValue (0, 0, 0, 1, 0, 0, 0, 0),
							STOrderFlagOut = GetBitsToByteValue (0, 0, 0, 0, 0, 0, 0, 0),
						},
						obj_add_order_flg = new ADDITIONAL_ORDER_FLAGS {
							Reserved1 = 2
						},
						TokenNo = obj_1.TokenNo,
						Contr_dec_tr_Obj = {
							InstrumentName = obj_1.Contr_dec_tr_Obj.InstrumentName,
							Symbol = obj_1.Contr_dec_tr_Obj.Symbol,
							ExpiryDate = obj_1.Contr_dec_tr_Obj.ExpiryDate,
							StrikePrice = obj_1.Contr_dec_tr_Obj.StrikePrice,
							OptionType = obj_1.Contr_dec_tr_Obj.OptionType
						},
						AccountNumber = obj_1.AccountNumber,
						Buy_SellIndicator = obj_1.Buy_SellIndicator,
						DisclosedVolume = obj_1.DisclosedVolume,
						Volume = obj_1.Volume,//IPAddress.NetworkToHostOrder(obj_1), //obj_1.Volume,
						Price = obj_1.Price,
						Open_Close = obj_1.Open_Close,
						UserId = obj_1.UserId, //IPAddress.NetworkToHostOrder (30072),
						BranchId = IPAddress.NetworkToHostOrder ((short)4),
						TraderId = obj_1.UserId, //IPAddress.NetworkToHostOrder (30072),
						BrokerId = Encoding.ASCII.GetBytes ("12468"),
						Settlor = Encoding.ASCII.GetBytes ("".ToUpper ().PadRight (12)),
						Pro_ClientIndicator = IPAddress.NetworkToHostOrder ((short)2),
						nnffield = obj_1.nnffield,

					};
					Console.WriteLine ("20000 Volume"+IPAddress.HostToNetworkOrder(obj_2.Volume));
					obj_2.Pro_ClientIndicator = IPAddress.NetworkToHostOrder ((short)2);
					count += 1;
					obj_2.OrderNumber = DoubleEndianChange (Convert.ToInt64 (DateTime.Now.ToString ("HHmmssfff")) + count);
					//  obj_2.OrderNumber =(long) DoubleEndianChange( 1100100258 + count);
					//Console.WriteLine(" Buy_SellIndicator  "+IPAddress.HostToNetworkOrder(obj_2.Buy_SellIndicator));
					concurrent.TryAdd ((long)1100100258 + count, obj_2);
					sendbuffer (Logic.Instance.GetPF (DataPacket.RawSerialize (obj_2)));
					if (!UDP_RecieverData.Instance.TokenData.ContainsKey (IPAddress.HostToNetworkOrder (obj_1.TokenNo))) {
						UDP_RecieverData.Instance.TokenData [IPAddress.HostToNetworkOrder (obj_1.TokenNo)] = new FinalPrice ();
					}
						FinalPrice _objFinal=new FinalPrice();
					if (UDP_RecieverData.Instance.TokenData.ContainsKey (IPAddress.HostToNetworkOrder (obj_1.TokenNo))) {
							
						_objFinal = UDP_RecieverData.Instance.TokenData [IPAddress.HostToNetworkOrder (obj_1.TokenNo)];
					}
					if (IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator) == 1 && IPAddress.HostToNetworkOrder (obj_1.Price) == 0)
					{
						Console.WriteLine (" 20000 Client Price  0:"+IPAddress.HostToNetworkOrder (obj_1.Price)
								+" Market Bid Price "+ _objFinal.MAXBID
								+" Market Ask Price "+ _objFinal.MINASK
								+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator)
							);
						obj_2.Price =IPAddress.NetworkToHostOrder( _objFinal.MINASK);
						sendbuffer (Logic.Instance.GetPF (tradeconferm_manual (obj_2)));
						return;
					}
					else if (IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator) == 2 && IPAddress.HostToNetworkOrder (obj_1.Price) == 0)
					{
							Console.WriteLine (" 20000 Client Price  0:"+IPAddress.HostToNetworkOrder (obj_1.Price)
								+" Market Bid Price "+ _objFinal.MAXBID
								+" Market Ask Price "+ _objFinal.MINASK
								+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator)
								+"Tokenno"+IPAddress.HostToNetworkOrder (obj_1.TokenNo)
							);
						obj_2.Price =IPAddress.NetworkToHostOrder( _objFinal.MAXBID);
						sendbuffer (Logic.Instance.GetPF (tradeconferm_manual (obj_2)));
						return;
					}
					if (IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator) == 1) {
						if (IPAddress.HostToNetworkOrder (obj_1.Price) >= _objFinal.MINASK && _objFinal.MINASK>0) {
								Console.WriteLine (" 20000 Client Price without && if:"+IPAddress.HostToNetworkOrder (obj_1.Price)
									+" Market Bid Price "+ _objFinal.MAXBID
									+" Market Ask Price "+ _objFinal.MINASK
									+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator)
									+"Tokenno"+IPAddress.HostToNetworkOrder (obj_1.TokenNo)
								);
								obj_2.Price =IPAddress.NetworkToHostOrder( _objFinal.MINASK);
							sendbuffer (Logic.Instance.GetPF (tradeconferm_manual (obj_2)));
							}
		
					}

					if (IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator) == 2) {

							if (IPAddress.HostToNetworkOrder (obj_1.Price) <= _objFinal.MAXBID) {

								Console.WriteLine (" 20000 Client Price without && if:"+IPAddress.HostToNetworkOrder (obj_1.Price)
									+" Market Bid Price "+ _objFinal.MAXBID
									+" Market Ask Price "+ _objFinal.MINASK
									+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1.Buy_SellIndicator)
									+"Tokenno"+IPAddress.HostToNetworkOrder (obj_1.TokenNo)
								);
								obj_2.Price =IPAddress.NetworkToHostOrder( _objFinal.MAXBID);
								sendbuffer (Logic.Instance.GetPF (tradeconferm_manual (obj_2)));

						}
					}



				
                        break;
				case 20040:
						var obj_1ms = (MS_OM_REQUEST_TR)DataPacket.RawDeserialize (buffer, typeof(MS_OM_REQUEST_TR));
					if (UDP_RecieverData.Instance.TokenData.ContainsKey (IPAddress.HostToNetworkOrder (obj_1ms.TokenNo))) {
						var _MobjFinal = UDP_RecieverData.Instance.TokenData [IPAddress.HostToNetworkOrder (obj_1ms.TokenNo)];
					
						sendbuffer (Logic.Instance.GetPF (modifyfunction (obj_1ms)));
						if (IPAddress.HostToNetworkOrder (obj_1ms.Buy_SellIndicator) == 1) {

								if (IPAddress.HostToNetworkOrder (obj_1ms.Price) >= _MobjFinal.MINASK && _MobjFinal.MINASK > 0) {
									Console.WriteLine ("Client Price 20040 :" + IPAddress.HostToNetworkOrder (obj_1ms.Price) 
										+ " Market Ask Price " + _MobjFinal.MINASK
										+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1ms.Buy_SellIndicator)
										+"TokenNO "+IPAddress.HostToNetworkOrder (obj_1ms.TokenNo)
									);
									obj_1ms.Price =IPAddress.NetworkToHostOrder( _MobjFinal.MINASK);
								sendbuffer (Logic.Instance.GetPF (tradeconferm_Mod (obj_1ms)));
							} 
						} else {

								if (IPAddress.HostToNetworkOrder (obj_1ms.Price) <= _MobjFinal.MAXBID) {

									Console.WriteLine ("Client Price 20040 :" + IPAddress.HostToNetworkOrder (obj_1ms.Price) 
										+ " Market Ask Price " + _MobjFinal.MAXBID
										+"Buy_SellIndicator "+IPAddress.HostToNetworkOrder (obj_1ms.Buy_SellIndicator)
										+"TokenNO "+IPAddress.HostToNetworkOrder (obj_1ms.TokenNo)
									);
									obj_1ms.Price =IPAddress.NetworkToHostOrder( _MobjFinal.MAXBID);
								sendbuffer (Logic.Instance.GetPF (tradeconferm_Mod (obj_1ms)));
							} 
						}

						//Console.WriteLine (" 20074 "+DoubleEndianChange(obj_1ms.OrderNumber));

					}

                        break;
                    case 20070:
                        var obj_1cal = (MS_OM_REQUEST_TR)DataPacket.RawDeserialize(buffer, typeof(MS_OM_REQUEST_TR));
					sendbuffer(Logic.Instance.GetPF(cancelfunction(obj_1cal)));
						Console.WriteLine (" 20075 "+DoubleEndianChange(obj_1cal.OrderNumber)+" Tokenno"+IPAddress.HostToNetworkOrder (obj_1cal.TokenNo));
					break;

				case 2100:
					MS_SPD_OE_REQUEST msoespd = (MS_SPD_OE_REQUEST)DataPacket.RawDeserialize (buffer, typeof(MS_SPD_OE_REQUEST));
					msoespd.OrderNumber1 = DoubleEndianChange (Convert.ToInt64 (DateTime.Now.ToString ("HHmmssfff")) + count);
                        //    msoespd.OrderNumber1 = DoubleEndianChange(1110111258 + count);
                       /* if (count % 2 == 0)
                        {
                            msoespd.header_obj.TransactionCode = IPAddress.NetworkToHostOrder((short)2127);
                            sendbuffer(Logic.Instance.GetPF(DataPacket.RawSerialize(msoespd)));
                            return;
                        }*/
					++count;
					Console.WriteLine ("BuySell1 "+IPAddress.HostToNetworkOrder(msoespd.BuySell1));
					Console.WriteLine ("leg2 BuySell2 "+IPAddress.HostToNetworkOrder(msoespd.leg2.BuySell2));
                        msoespd.header_obj.TransactionCode = IPAddress.NetworkToHostOrder((short)2124);
                        sendbuffer(Logic.Instance.GetPF(DataPacket.RawSerialize(msoespd)));

                        tradeconfermiocistleg(msoespd);//
					sendbuffer(Logic.Instance.GetPF(tradeconfermioc2ndleg(msoespd)));

                        //sendbuffer(Logic.Instance.GetPF(tradeconfermioc2ndleg(msoespd)));
					//sendbuffer (Logic.Instance.GetPF (tradeconferm_manual (msoespd)));
                        break;
                }
            }
            else
            {
                var _checkval = (Message_Header)DataPacket.RawDeserialize(buffer, typeof(Message_Header));
                int trcode = IPAddress.NetworkToHostOrder(_checkval.TransactionCode);
            }
        }
		}

        private int tradectfillno = 0;
		Random R_obj=new Random();
		int FillNumber =142568;
		public byte[] tradeconferm_manual(MS_OE_RESPONSE_TR obj)//,int qty,int rem)
		{

			var obj1 = new MS_TRADE_CONFIRM_TR()
			{

				TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
				ResponseOrderNumber = obj.OrderNumber,

				Contr_dec_tr_Obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType,

				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				FillQuantity =obj.Volume,
			
				Price =obj.Price,
				OpenClose = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = obj.BrokerId,
				Token = obj.TokenNo,
				VolumeFilledToday = obj.VolumeFilledToday,
				RemainingVolume =obj.TotalVolumeRemaining,
				FillNumber = IPAddress.NetworkToHostOrder(FillNumber),

				FillPrice=obj.Price,

			};
			FillNumber += 1;
			//Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume));
			Console.WriteLine ("Buy_SellIndicator 20222 = "+IPAddress.HostToNetworkOrder(obj.Buy_SellIndicator)+" price :"+IPAddress.HostToNetworkOrder(obj1.FillPrice));
		//	sendbuffer (Logic.Instance.GetPF (tradeconferm_manual2(obj)));

			//obj1.FillQuantity = IPAddress.HostToNetworkOrder (25);
			return DataPacket.RawSerialize(obj1);
		}


		private byte[] cancelfunction(MS_OM_REQUEST_TR obj)
		{
			var obj1 = new MS_OE_RESPONSE_TR()
			{
				TransactionCode = IPAddress.NetworkToHostOrder((short)(20075)),
				OrderNumber = obj.OrderNumber,
				Contr_dec_tr_Obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType
				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				Volume = obj.Volume,
				Price = obj.Price,
				Open_Close = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = obj.BrokerId,
				nnffield = obj.nnffield,
				TokenNo = obj.TokenNo
			};
			MS_OE_RESPONSE_TR b = new MS_OE_RESPONSE_TR();
			concurrent.TryRemove((long)DoubleEndianChange(obj.OrderNumber), out b);
			return DataPacket.RawSerialize(obj1);
		}

		public byte[] tradeconferm_manual2(MS_OE_RESPONSE_TR obj)
		{
			var obj1 = new MS_TRADE_CONFIRM_TR()
			{

				TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
				ResponseOrderNumber = obj.OrderNumber,

				Contr_dec_tr_Obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType,

				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				FillQuantity =IPAddress.NetworkToHostOrder(IPAddress.HostToNetworkOrder(obj.Volume)-25),

				Price =IPAddress.NetworkToHostOrder(R_obj.Next(10000,100000)), //obj.Price,
				OpenClose = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = obj.BrokerId,
				Token = obj.TokenNo,
				VolumeFilledToday = obj.VolumeFilledToday,
				RemainingVolume = obj.TotalVolumeRemaining,
				FillNumber = IPAddress.NetworkToHostOrder(FillNumber),

				FillPrice=IPAddress.NetworkToHostOrder(R_obj.Next(10000,100000)),

			};
			FillNumber += 1;
			//Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume));
			Console.WriteLine ("Buy_SellIndicator 20222 = "+IPAddress.HostToNetworkOrder(obj.Buy_SellIndicator));

			return DataPacket.RawSerialize(obj);
		}

        public void tradeconfermiocistleg(MS_SPD_OE_REQUEST obj) // 
        // public byte[] tradeconfermiocistleg(MS_SPD_OE_REQUEST obj)
        {
			Console.WriteLine ("IPAddress.NetworkToHostOrder(obj.Volume1)"+IPAddress.NetworkToHostOrder(obj.Volume1));
            int LotCount = IPAddress.NetworkToHostOrder(obj.Volume1) / 25;//

            var obj1 = new MS_TRADE_CONFIRM_TR()
            {

                TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
                ResponseOrderNumber = obj.OrderNumber1,

                Contr_dec_tr_Obj =
                {
                    InstrumentName = obj.ms_oe_obj.InstrumentName,
                    Symbol = obj.ms_oe_obj.Symbol,
                    ExpiryDate = obj.ms_oe_obj.ExpiryDate,
                    StrikePrice = obj.ms_oe_obj.StrikePrice,
                    OptionType = obj.ms_oe_obj.OptionType,
                },
                AccountNumber = obj.AccountNumber1,

                Buy_SellIndicator = obj.BuySell1,
                DisclosedVolume = obj.DisclosedVolRemaining1,
				FillQuantity = obj.Volume1,//
                Price = obj.Price1,
                OpenClose = obj.OpenClose1,
                TraderId = obj.TraderId1,
                BrokerId = obj.BrokerId1,
                Token = obj.Token1,
                VolumeFilledToday = obj.VolumeFilledToday1,
                RemainingVolume = obj.TotalVolRemaining1,
                FillNumber = IPAddress.NetworkToHostOrder(tradectfillno),
                LogTime = (int)Logic.Instance.ConvertToTimestamp(System.DateTime.Now),
                FillPrice = obj.Price1,
                GoodTillDate = obj.GoodTillDate1,
                Timestamp = 0,
                Reserved = obj.obj_add_order_flg.Reserved1,
				Timestamp1 = IPAddress.NetworkToHostOrder(ToEpochTime(DateTime.Now)),
				Timestamp2 = IPAddress.NetworkToHostOrder(ToEpochTime(DateTime.Now)),
                OriginalVolume = obj.Volume1,

            };
			Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume1));
			Console.WriteLine ("FillQuantity2  "+obj.Volume1);
            var VolFillToday = 0;
            while (LotCount > 0)//
            {
                VolFillToday = VolFillToday + IPAddress.HostToNetworkOrder(obj1.FillQuantity);
                obj1.VolumeFilledToday = IPAddress.HostToNetworkOrder(VolFillToday);
                sendbuffer(Logic.Instance.GetPF(DataPacket.RawSerialize(obj1)));
                obj1.FillPrice = IPAddress.HostToNetworkOrder(IPAddress.NetworkToHostOrder(obj.Price1) + 50);
                --LotCount;
            }
            // return DataPacket.RawSerialize(obj1);
        }


        public byte[] tradeconfermioc2ndleg(MS_SPD_OE_REQUEST obj)
        {

            var obj1 = new MS_TRADE_CONFIRM_TR()
            {

                TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
                ResponseOrderNumber = obj.OrderNumber1,

                Contr_dec_tr_Obj =
                {
                    InstrumentName = obj.leg2.ms_oe_obj.InstrumentName,
                    Symbol = obj.leg2.ms_oe_obj.Symbol,
                    ExpiryDate = obj.leg2.ms_oe_obj.ExpiryDate,
                    StrikePrice = obj.leg2.ms_oe_obj.StrikePrice,
                    OptionType = obj.leg2.ms_oe_obj.OptionType,

                },
                AccountNumber = obj.AccountNumber1,
				Buy_SellIndicator = obj.leg2.BuySell2,
                DisclosedVolume = obj.DisclosedVolRemaining1,
                FillQuantity = obj.leg2.Volume2,
                Price = obj.leg2.Price2,
                OpenClose = obj.leg2.OpenClose2,
                TraderId = obj.TraderId1,
                BrokerId = obj.BrokerId1,
                Token = obj.leg2.token,
                VolumeFilledToday = obj.leg2.Volume2,

                RemainingVolume = obj.leg2.TotalVolRemaining2,
                FillNumber = IPAddress.NetworkToHostOrder(++tradectfillno),
                LogTime = (int)Logic.Instance.ConvertToTimestamp(System.DateTime.Now),
                FillPrice = obj.leg2.Price2,
                GoodTillDate = obj.GoodTillDate1,
                Timestamp = 0,
                Reserved = obj.obj_add_order_flg.Reserved1,
                Timestamp1 = 0,
                Timestamp2 = 0,


            };
			Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume1));
			Console.WriteLine ("FillQuantity2  "+obj.Volume1);
            return DataPacket.RawSerialize(obj1);
        }

        public byte[] logicclass(MS_OM_REQUEST_TR obj_1ms)
        {
			return tradeconferm(obj_1ms);
            if (_concurrentDataDictionary.TryGetValue(IPAddress.HostToNetworkOrder(obj_1ms.TokenNo), out _TempFP) == true)
            {


                if (IPAddress.HostToNetworkOrder(obj_1ms.Buy_SellIndicator) == 1)
                {
                    if (IPAddress.HostToNetworkOrder(obj_1ms.Price) <= _concurrentDataDictionary[IPAddress.HostToNetworkOrder(obj_1ms.TokenNo)].MAXBID)
                    {
                        MS_OE_RESPONSE_TR obj_21 = new MS_OE_RESPONSE_TR
                        {
                            TransactionCode = IPAddress.NetworkToHostOrder((short)20073),
                            ReasonCode = IPAddress.NetworkToHostOrder((short)0),
                            BookType = IPAddress.NetworkToHostOrder((short)1),
                            GoodTillDate = IPAddress.NetworkToHostOrder(0),
                            st_ord_flg_obj = new ST_ORDER_FLAGS
                            {
                                STOrderFlagIn = GetBitsToByteValue(0, 0, 0, 1, 0, 0, 0, 0),
                                STOrderFlagOut = GetBitsToByteValue(0, 0, 0, 0, 0, 0, 0, 0),
                            },
                            obj_add_order_flg = new ADDITIONAL_ORDER_FLAGS
                            {
                                Reserved1 = 2
                            },
                            TokenNo = obj_1ms.TokenNo,
                            Contr_dec_tr_Obj =
                            {
                                InstrumentName = obj_1ms.Contr_dec_tr_Obj.InstrumentName,
                                Symbol = obj_1ms.Contr_dec_tr_Obj.Symbol,
                                ExpiryDate = obj_1ms.Contr_dec_tr_Obj.ExpiryDate,
                                StrikePrice = obj_1ms.Contr_dec_tr_Obj.StrikePrice,
                                OptionType = obj_1ms.Contr_dec_tr_Obj.OptionType
                            },
                            AccountNumber = obj_1ms.AccountNumber,
                            Buy_SellIndicator = obj_1ms.Buy_SellIndicator,
                            DisclosedVolume = obj_1ms.DisclosedVolume,
                            Volume = obj_1ms.Volume,
                            Price = obj_1ms.Price,
                            Open_Close = obj_1ms.Open_Close,
							UserId = obj_1ms.UserId, //IPAddress.NetworkToHostOrder(30072),
                            BranchId = IPAddress.NetworkToHostOrder((short)4),
                            TraderId = IPAddress.NetworkToHostOrder(30072),
                            BrokerId = Encoding.ASCII.GetBytes("12468"),
                            Settlor = Encoding.ASCII.GetBytes("".ToUpper().PadRight(12)),
                            Pro_ClientIndicator = IPAddress.NetworkToHostOrder((short)2),
                            nnffield = obj_1ms.nnffield,
                        };
                        obj_1ms.Pro_ClientIndicator = IPAddress.NetworkToHostOrder((short)2);
                        _concortdict[IPAddress.NetworkToHostOrder(obj_1ms.TokenNo)][IPAddress.NetworkToHostOrder(obj_1ms.Buy_SellIndicator)].AddOrUpdate(IPAddress.NetworkToHostOrder((long)obj_1ms.OrderNumber), obj_21, (k, d) => obj_21);
						return modifyfunction(obj_1ms);
                    }
                    else if (IPAddress.HostToNetworkOrder(obj_1ms.Price) > _concurrentDataDictionary[IPAddress.HostToNetworkOrder(obj_1ms.TokenNo)].MAXBID)
                    {
                        MS_OE_RESPONSE_TR orresponce = new MS_OE_RESPONSE_TR();
                        _concortdict[IPAddress.NetworkToHostOrder(obj_1ms.TokenNo)][IPAddress.NetworkToHostOrder(obj_1ms.Buy_SellIndicator)].TryRemove(IPAddress.NetworkToHostOrder((long)obj_1ms.OrderNumber), out orresponce);
                        return tradeconferm(obj_1ms);
                    }
                }
                else if (IPAddress.HostToNetworkOrder(obj_1ms.Buy_SellIndicator) == 2)
                {
                    if (IPAddress.HostToNetworkOrder(obj_1ms.Price) >= _concurrentDataDictionary[IPAddress.HostToNetworkOrder(obj_1ms.TokenNo)].MINASK)
                    {
                        return modifyfunction(obj_1ms);
                    }
                    else if (IPAddress.HostToNetworkOrder(obj_1ms.Price) < _concurrentDataDictionary[IPAddress.HostToNetworkOrder(obj_1ms.TokenNo)].MINASK)
                    {
                        return tradeconferm(obj_1ms);
                    }
                }
            }
            byte[] buff = { };
            return buff;
        }
        public byte[] tradeconferm(MS_OM_REQUEST_TR obj)
        {

            var obj1 = new MS_TRADE_CONFIRM_TR()
            {

                TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
                ResponseOrderNumber = obj.OrderNumber,

                Contr_dec_tr_Obj =
                {
                    InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
                    Symbol = obj.Contr_dec_tr_Obj.Symbol,
                    ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
                    StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
                    OptionType = obj.Contr_dec_tr_Obj.OptionType,

                },
                AccountNumber = obj.AccountNumber,
                Buy_SellIndicator = obj.Buy_SellIndicator,
                DisclosedVolume = obj.DisclosedVolume,
                FillQuantity = obj.Volume,
                Price = obj.Price,
                OpenClose = obj.Open_Close,
                TraderId = obj.TraderId,
                BrokerId = obj.BrokerId,
                Token = obj.TokenNo,
                VolumeFilledToday = obj.VolumeFilledToday,
                RemainingVolume = obj.TotalVolumeRemaining
            };
			Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume));
			Console.WriteLine ("FillQuantity2  "+obj.Volume);
            return DataPacket.RawSerialize(obj1);
        }


        private byte[] modifyfunction(MS_OM_REQUEST_TR obj)
        {
            var obj1 = new MS_OE_RESPONSE_TR()
            {
                TokenNo = obj.TokenNo,
                nnffield = obj.nnffield,
                TransactionCode = IPAddress.NetworkToHostOrder((short)(20074)),
                OrderNumber = obj.OrderNumber,
                Contr_dec_tr_Obj =
                {
                    InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
                    Symbol = obj.Contr_dec_tr_Obj.Symbol,
                    ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
                    StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
                    OptionType = obj.Contr_dec_tr_Obj.OptionType
                },
                AccountNumber = obj.AccountNumber,
                Buy_SellIndicator = obj.Buy_SellIndicator,
                DisclosedVolume = obj.DisclosedVolume,
                Volume = obj.Volume,
                Price = obj.Price,
                Open_Close = obj.Open_Close,
                TraderId = obj.TraderId,
                BrokerId = Encoding.ASCII.GetBytes(obj.TraderId.ToString()),
            };
            concurrent.TryUpdate((long)1100100258 + count, obj1, obj1);
			Console.WriteLine ("20040 Volume"+IPAddress.HostToNetworkOrder(obj1.Volume));
            return DataPacket.RawSerialize(obj1);
        }


		public byte[] tradeconferm_Mod(MS_OM_REQUEST_TR obj)//,int qty,int rem)
		{

			var obj1 = new MS_TRADE_CONFIRM_TR()
			{

				TransactionCode = IPAddress.NetworkToHostOrder((short)(20222)),
				ResponseOrderNumber = obj.OrderNumber,

				Contr_dec_tr_Obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType,

				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				FillQuantity =obj.Volume,

				Price =obj.Price,
				OpenClose = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = obj.BrokerId,
				Token = obj.TokenNo,
				VolumeFilledToday = obj.VolumeFilledToday,
				RemainingVolume =obj.TotalVolumeRemaining,
				FillNumber = IPAddress.NetworkToHostOrder(FillNumber),

				FillPrice=obj.Price,

			};
			FillNumber += 1;
			//Console.WriteLine ("FillQuantity1  "+IPAddress.HostToNetworkOrder(obj.Volume));
			Console.WriteLine ("Buy_SellIndicator 20222 = "+IPAddress.HostToNetworkOrder(obj.Buy_SellIndicator)+" price :"+IPAddress.HostToNetworkOrder(obj1.FillPrice));
			//	sendbuffer (Logic.Instance.GetPF (tradeconferm_manual2(obj)));

			//obj1.FillQuantity = IPAddress.HostToNetworkOrder (25);
			return DataPacket.RawSerialize(obj1);
		}

		//private byte[] cancelfunction(MS_OM_REQUEST_TR obj)
		private byte[] cancelmodifyfunction(MS_OE_REQUEST_TR obj)
		{
			var obj1 = new MS_OE_REQUEST()
			{
				header_obj =
				{
					TransactionCode = Convert.ToInt16(20072),
				},

				OrderNumber = 1100100258 + count,
				contract_obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType
				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				Volume = obj.Volume,
				Price = obj.Price,
				Open_Close = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = Encoding.ASCII.GetBytes(obj.TraderId.ToString())
			};

			return DataPacket.RawSerialize(obj1);
		}
       
		private byte[] rejectmodifyfunction(MS_OM_REQUEST_TR obj)
        {
            var obj1 = new MS_OE_REQUEST()
            {
                header_obj =
                {
					TransactionCode =IPAddress.HostToNetworkOrder(Convert.ToInt16(2042)),
                    ErrorCode = 0,
                    LogTime = 1,
                    MessageLength = 1024,
                },
                OrderNumber = 1100100258 + count,
                contract_obj =
                {
                    InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
                    Symbol = obj.Contr_dec_tr_Obj.Symbol,
                    ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
                    StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
                    OptionType = obj.Contr_dec_tr_Obj.OptionType
                },
                AccountNumber = obj.AccountNumber,
                Buy_SellIndicator = obj.Buy_SellIndicator,
                DisclosedVolume = obj.DisclosedVolume,
                Volume = obj.Volume,
                Price = obj.Price,
                Open_Close = obj.Open_Close,
                TraderId = obj.TraderId,
                BrokerId = Encoding.ASCII.GetBytes(obj.TraderId.ToString())
            };
            return DataPacket.RawSerialize(obj1);
        }

		private byte[] rejectCancelfunction(MS_OM_REQUEST_TR obj)
		{
			var obj1 = new MS_OE_REQUEST()
			{
				header_obj =
				{
					TransactionCode =IPAddress.HostToNetworkOrder(Convert.ToInt16(2072)),
					ErrorCode = 0,
					LogTime = 1,
					MessageLength = 1024,
				},
				OrderNumber = 1100100258 + count,
				contract_obj =
				{
					InstrumentName = obj.Contr_dec_tr_Obj.InstrumentName,
					Symbol = obj.Contr_dec_tr_Obj.Symbol,
					ExpiryDate = obj.Contr_dec_tr_Obj.ExpiryDate,
					StrikePrice = obj.Contr_dec_tr_Obj.StrikePrice,
					OptionType = obj.Contr_dec_tr_Obj.OptionType
				},
				AccountNumber = obj.AccountNumber,
				Buy_SellIndicator = obj.Buy_SellIndicator,
				DisclosedVolume = obj.DisclosedVolume,
				Volume = obj.Volume,
				Price = obj.Price,
				Open_Close = obj.Open_Close,
				TraderId = obj.TraderId,
				BrokerId = Encoding.ASCII.GetBytes(obj.TraderId.ToString())
			};
			return DataPacket.RawSerialize(obj1);
		}

        public byte[] stpformat()
        {
            var _StrPacketFormate = new StrPacketFormate
            {
                Length = (short)1,
                SequenceNumber = 1,
                ResrvSequenceNumber = Encoding.ASCII.GetBytes("ab"),
                CheckSum = Encoding.ASCII.GetBytes("1234567898745632"),
                MsgCount = 1,


            };

            byte[] b = DataPacket.RawSerialize(_StrPacketFormate);
            int y = b.Length;
            return b;
        }

        protected byte[] readStream(TcpClient client)
        {
            NetworkStream stream = client.GetStream();
            if (stream.DataAvailable)
            {
                byte[] data = new byte[client.Available];

                int bytesRead = 0;
                try
                {
                    bytesRead = stream.Read(data, 0, data.Length);
                }
                catch (IOException)
                {
                }

                if (bytesRead < data.Length)
                {
                    byte[] lastData = data;
                    data = new byte[bytesRead];
                    Array.ConstrainedCopy(lastData, 0, data, 0, bytesRead);
                }
                return data;
            }
            return null;
        }

        private void tcpServer1_OnConnect(tcpServer.TcpServerConnection connection)
        {
            iccount.Invoke();
            Console.WriteLine("Server Connect Successfully . \n Continue..");
            //invokeDelegate setText = () => lblConnected.Text = tcpServer1.Connections.Count.ToString();
            //Invoke(setText);
        }
        static void Main(string[] args)
        {
            Program p = new Program();
            Console.ReadLine();

        }
    }
    public class Logic
    {
        [DllImport("libMd5CheckSum.so", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void md5CheckSum(byte[] Buffer, int BufferSize, byte[] dstBuffer);


        public int InvitationCount = 0, SeqNo = 0;


        public MD5 Md5Hash = MD5.Create();


        private static readonly Logic instance = new Logic();
        public static Logic Instance
        {
            get
            {
                return instance;
            }
        }

        private Logic()
        {

            Md5Hash = MD5.Create();

            //   new Config {GroupName = null};
        }


        public double ConvertToTimestamp(DateTime date)
        {
            return Math.Floor((date - new DateTime(1980, 1, 1, 0, 0, 0, 0)).TotalSeconds);
        }


        public byte[] GetMD5(byte[] _buffer)
        {


            return Md5Hash.ComputeHash(_buffer, 0, _buffer.Length);
            //object modLock=new object();
            //lock (modLock) {
            //    var buffer=_buffer;
            //    byte[] md5buff = new byte[16];
            //    md5CheckSum (buffer, buffer.Length, md5buff);
            //    return md5buff;
            //  }

        }
        public byte[] GetPF(byte[] _buffer)
        {


            var pf = new StrPacketFormate
            {

                SequenceNumber = IPAddress.HostToNetworkOrder(Logic.Instance.SeqNo >= int.MaxValue ? 1 : ++Logic.Instance.SeqNo),
                Length = IPAddress.HostToNetworkOrder((short)(26 + _buffer.Length)),
            };
            var bu1 = DataPacket.RawSerialize(pf);
            var buffer = bu1.Concat(_buffer).ToArray();

            Logic.Instance.GetMD5(buffer.Skip(24).Take(buffer.Length - 24).ToArray()).CopyTo(buffer, 8);
            return buffer;
        }


        public DateTime ConvertFromTimestamp(double timestamp)
        {
            return new DateTime(1980, 1, 1, 0, 0, 0, 0).AddSeconds(timestamp);
        }

        public double DoubleEndianChange(double value)
        {
            return BitConverter.ToDouble(BitConverter.GetBytes(value).Reverse().ToArray(), 0);
        }

        public byte GetBitsToByteValue(byte bit0, byte bit1, byte bit2, byte bit3, byte bit4, byte bit5, byte bit6, byte bit7)
        {
            var numArray1 = new[]
			{
				bit0,
				bit1, 
				bit2,
				bit3,
				bit4,
				bit5,
				bit6,
				bit7
			};
            var numArray2 = numArray1;
            byte num = 0;
            for (byte index = 0; (int)index < 8; ++index)
                num |= (int)numArray2[index] == 1 ? (byte)(1 << index) : (byte)0;
            return num;
        }

        public byte[] GetBytes(string str)
        {

            //Apply _Objbuf.Take(bytes.Length);

            var bytes = new byte[str.Length * sizeof(char)];
            Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }




    }
}
