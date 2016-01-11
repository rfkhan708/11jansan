using System;
using System.Diagnostics;
using System.Threading;
using Structure;
using AMS.Profile;
using System.Collections.Specialized;
using System.Configuration;
using System.Runtime.InteropServices;
namespace simuletionconsole
{
	public sealed class Global
	{
		internal string SERVER_IP;
		internal string SERVER_PORT;
		internal string DATA_IP;
		internal string DATA_PORT;
		private static readonly Global instance = new Global ();
		public static Global Instance
		{
			get{
				return instance;
			}
		}

		private Global()
		{
			SERVER_IP=ReadSetting("SERVER_IP");
			SERVER_PORT=ReadSetting( "SERVER_PORT");
			DATA_IP=ReadSetting( "DATA_IP");
			DATA_PORT=ReadSetting( "DATA_PORT");

		}

		public string ReadSetting(string key)
		{
			string result = null;
			try
			{

				//NameValueCollection appSettings = ConfigurationManager.AppSettings;

				//result = appSettings[key] ?? "Not Found";
				//Console.WriteLine(result);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error reading app settings");
			}
			return result;
		}


	}

}


