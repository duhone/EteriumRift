using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace Eterium
{
    public class OculusGraphicsDeviceManager : GraphicsDeviceManager
    {
        public OculusGraphicsDeviceManager(Game _game) : base(_game) {}
        public string OculusDeviceName { get; set; }
        public bool DebugMode { get; set; }

        protected override void RankDevices(List<GraphicsDeviceInformation> _foundDevices)
        {
            base.RankDevices(_foundDevices);

            if (!DebugMode)
            {
                if (OculusDeviceName == null || OculusDeviceName.Length == 0)
                    return;

                _foundDevices.RemoveAll(info =>
                {
                    string deviceName = info.Adapter.DeviceName;
                    if (OculusDeviceName.Contains(deviceName))
                        return false;
                    else
                        return true;
                });

                if (_foundDevices.Count == 0)
                    throw new Exception("Could not find an active oculus display");
            }
        }
    }
}
