		
/*************************************************************
need a few member variables in your game class. use the graphics device manager in the examples folder.
**************************************************************/
OculusGraphicsDeviceManager m_Graphics;
RE.Oculus.DeviceConfig m_oculusCfg;
		
/*************************************************************
This code would go in your game class constructor
**************************************************************/
    GameComponents.m_OculusDeviceManager = new RE.Oculus.DeviceManager();
				
	bool createdDevice = false;
	int selectedDevice = -1;
	GameComponents.m_OculusDeviceManager.EnumerateDevices(Globals.DisplaySettings.s_RiftQuality, _deviceInfo =>
		{
			if (!createdDevice) //just always taking first device for now
			{
				createdDevice = true;
				m_Graphics.PreferredBackBufferWidth = _deviceInfo.Resolution.X;
				m_Graphics.PreferredBackBufferHeight = _deviceInfo.Resolution.Y;

				Globals.s_ScreenSize.X = _deviceInfo.RecommendedTextureResolutionLeft.X;
				Globals.s_ScreenSize.Y = _deviceInfo.RecommendedTextureResolutionLeft.Y;
				m_Graphics.OculusDeviceName = _deviceInfo.DeviceName;

				selectedDevice = _deviceInfo.DisplayID;
			}
		});
	if (!createdDevice)
	{
		MessageBox.Show("Oculus device not found");
		throw new System.Exception("Oculus device not found");
	}

	//m_oculusCfg.Device = GraphicsDevice;
	m_oculusCfg.DisplayID = selectedDevice;
	m_oculusCfg.GameInstance = this;
	m_oculusCfg.RenderTargetResolution.X = m_Graphics.PreferredBackBufferWidth;
	m_oculusCfg.RenderTargetResolution.Y = m_Graphics.PreferredBackBufferHeight;
	m_oculusCfg.RenderTextureResolutionLeft.X = (int)Globals.s_ScreenSize.X;
	m_oculusCfg.RenderTextureResolutionLeft.Y = (int)Globals.s_ScreenSize.Y;
	m_oculusCfg.RenderTextureResolutionRight.X = (int)Globals.s_ScreenSize.X;
	m_oculusCfg.RenderTextureResolutionRight.Y = (int)Globals.s_ScreenSize.Y;
	
    IsFixedTimeStep = false; //DK2 needs to run at 75fps
	
/*************************************************************
This code need to happen after xna creates the graphics device. LoadContent of game class isn't a bad spot.
**************************************************************/	
	m_oculusCfg.Device = GraphicsDevice;
	//LeftRiftTarget/RightRiftTarget is a RenderTarget2D. these just need to be your left/right eye final render targets
	m_oculusCfg.LeftRenderTexture = GameComponents.s_BloomComponent.LeftRiftTarget; 
	m_oculusCfg.RightRenderTexture = GameComponents.s_BloomComponent.RightRiftTarget;
	m_oculusCfg.zNear = 0.2f;
	m_oculusCfg.zFar = 10000; 
	GameComponents.m_OculusDevice = GameComponents.m_OculusDeviceManager.CreateDevice(m_oculusCfg);

	if (GameComponents.m_OculusDevice == null)
	{
		MessageBox.Show("Unable to initialize Oculus Rift");
		throw new System.Exception("Unable to initialize Oculus Rift");
	}
	
	
/*************************************************************
You need to override EndDraw in your game class. Prevent XNA from calling Present. let the rift sdk do that instead.
**************************************************************/	
	protected override void EndDraw()
	{
		if (Globals.DisplaySettings.s_RenderMode == Globals.DisplaySettings.RenderMode.Oculus)
		{
			GameComponents.m_OculusDevice.EndFrame();
		}
		else
		{
			base.EndDraw();
		}
	}
	
	
/*************************************************************
Need to render twice, once for each eye in your draw function in your game class.
**************************************************************/	
	protected override void Draw(GameTime _GameTime)
	{
		if (Globals.DisplaySettings.s_RenderMode == Globals.DisplaySettings.RenderMode.Oculus)
		{
			GameComponents.m_OculusDevice.Update();
			GameComponents.m_OculusDevice.BeginFrame();
			GameComponents.m_OculusDevice.BeginEyeRender(RE.Oculus.EYE.LEFT_EYE);
			var ssettings = GameComponents.m_OculusDevice.SSettings;
			Globals.s_PlayerController.Camera.CreateProjection(ssettings.LeftProjection);
			Globals.DisplaySettings.s_CurrentEye = 0;
			DrawImpl(_GameTime, 0);
			GameComponents.m_OculusDevice.EndEyeRender();
			GameComponents.m_OculusDevice.BeginEyeRender(RE.Oculus.EYE.RIGHT_EYE);
			ssettings = GameComponents.m_OculusDevice.SSettings;
			Globals.s_PlayerController.Camera.CreateProjection(ssettings.RightProjection);
			Globals.DisplaySettings.s_CurrentEye = 1;
			DrawImpl(_GameTime, 1);
			GameComponents.m_OculusDevice.EndEyeRender();
		}
		else
		{
			DrawImpl(_GameTime, 0);
		}
	}


/*************************************************************
Need to modify your view matrix. this is a a property on Eteriums camera class.
**************************************************************/		
	public Matrix View
	{
		get
		{
			if (Globals.DisplaySettings.s_RenderMode == Globals.DisplaySettings.RenderMode.Oculus)
			{
				var pose = GameComponents.m_OculusDevice.GetPose();
				if(Globals.DisplaySettings.s_CurrentEye == 0)
					return m_View * Matrix.CreateTranslation(pose.Position) * Matrix.CreateFromQuaternion(pose.Orientation) * Matrix.CreateTranslation(GameComponents.m_OculusDevice.SSettings.LeftViewAdjust);
				else
					return m_View * Matrix.CreateTranslation(pose.Position) * Matrix.CreateFromQuaternion(pose.Orientation) * Matrix.CreateTranslation(GameComponents.m_OculusDevice.SSettings.RightViewAdjust);
			}
			else
			{
				return m_View;
			}
		}
		set
		{
			m_View = value;
			Matrix.Invert(ref m_View, out m_Xform);
		}
	}