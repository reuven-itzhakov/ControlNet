import React, { useState, useEffect } from 'react';
import { Download, LogIn, Send, CheckCircle, ArrowRight } from 'lucide-react';
import { downloadAPI } from '../../services/api';

const AddDeviceGuide = () => {
  const [downloadInfo, setDownloadInfo] = useState(null);
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    // Fetch download information
    const fetchDownloadInfo = async () => {
      try {
        const info = await downloadAPI.getInfo();
        setDownloadInfo(info);
      } catch (error) {
        console.error('Error fetching download info:', error);
      }
    };
    
    fetchDownloadInfo();
  }, []);

  const handleDownload = () => {
    const downloadUrl = downloadAPI.getDownloadUrl();
    // Create a temporary anchor element to trigger download
    const link = document.createElement('a');
    link.href = downloadUrl;
    link.download = 'ControlNetClient.exe';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  return (
    <div id="add-device-guide" className="space-y-8">
      {/* Header */}
      <div className="text-center">
        <h2 className="text-3xl font-bold text-white mb-3">How to Add a Device</h2>
        <p className="text-gray-400 text-lg">Follow these simple steps to start tracking keystrokes on your device</p>
      </div>

      {/* Steps */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Step 1: Download */}
        <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6 hover:border-cyan-500/50 transition-all duration-300">
          <div className="flex items-start space-x-4 mb-6">
            <div className="w-12 h-12 bg-gradient-to-r from-cyan-500 to-blue-500 rounded-lg flex items-center justify-center flex-shrink-0">
              <Download className="w-6 h-6 text-white" />
            </div>
            <div className="flex-1">
              <div className="flex items-center space-x-2 mb-2">
                <h3 className="text-xl font-bold text-white">Step 1</h3>
                <span className="text-cyan-400 text-sm font-medium">Download Program</span>
              </div>
              <p className="text-gray-400">
                Download the ControlNet desktop application for your operating system
              </p>
            </div>
          </div>
          
          {/* Centered Download Button */}
          <div className="flex flex-col items-center justify-center py-8">
            <button
              onClick={handleDownload}
              disabled={!downloadInfo?.available}
              className={`inline-flex items-center space-x-2 px-6 py-3 rounded-lg transition-all duration-300 group ${
                downloadInfo?.available
                  ? 'bg-gradient-to-r from-cyan-500 to-blue-500 text-white hover:from-cyan-400 hover:to-blue-400 hover:scale-105 shadow-lg hover:shadow-cyan-500/50'
                  : 'bg-gray-600 text-gray-400 cursor-not-allowed'
              }`}
            >
              <Download className="w-5 h-5" />
              <span className="font-semibold">
                {downloadInfo?.available
                  ? `Download for Windows${downloadInfo?.sizeFormatted ? ` (${downloadInfo.sizeFormatted})` : ''}`
                  : 'Download Not Available'}
              </span>
              {downloadInfo?.available && (
                <ArrowRight className="w-5 h-5 group-hover:translate-x-1 transition-transform" />
              )}
            </button>
            {downloadInfo?.available && (
              <p className="text-gray-500 text-sm mt-3">
                Version {downloadInfo.version || '1.0.0'} • Windows 10/11 Compatible
              </p>
            )}
          </div>
        </div>

        {/* Step 2: Sign In */}
        <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-purple-500/30 rounded-xl p-6 hover:border-purple-500/50 transition-all duration-300">
          <div className="flex items-start space-x-4">
            <div className="w-12 h-12 bg-gradient-to-r from-purple-500 to-pink-500 rounded-lg flex items-center justify-center flex-shrink-0">
              <LogIn className="w-6 h-6 text-white" />
            </div>
            <div className="flex-1">
              <div className="flex items-center space-x-2 mb-2">
                <h3 className="text-xl font-bold text-white">Step 2</h3>
                <span className="text-purple-400 text-sm font-medium">Sign In</span>
              </div>
              <p className="text-gray-400 mb-4">
                Launch the program and sign in with your account credentials. This links the device to your dashboard.
              </p>
              <div className="space-y-2 text-sm">
                <div className="flex items-center space-x-2 text-gray-300">
                  <CheckCircle className="w-4 h-4 text-green-400" />
                  <span>Enter your email address</span>
                </div>
                <div className="flex items-center space-x-2 text-gray-300">
                  <CheckCircle className="w-4 h-4 text-green-400" />
                  <span>Enter your password</span>
                </div>
                <div className="flex items-center space-x-2 text-gray-300">
                  <CheckCircle className="w-4 h-4 text-green-400" />
                  <span>Device automatically registers</span>
                </div>
              </div>
            </div>
          </div>
          
          {/* Step 2 Image */}
          <div className="mt-6 flex justify-center">
            <div className="rounded-lg overflow-hidden border border-gray-600/30 max-w-xl">
              <img 
                src="/step 2.png" 
                alt="Sign in screen showing login form with email and password fields"
                className="w-full h-auto"
              />
            </div>
          </div>
        </div>

        {/* Step 3: Start Typing */}
        <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-green-500/30 rounded-xl p-6 hover:border-green-500/50 transition-all duration-300">
          <div className="flex items-start space-x-4">
            <div className="w-12 h-12 bg-gradient-to-r from-green-500 to-emerald-500 rounded-lg flex items-center justify-center flex-shrink-0">
              <Send className="w-6 h-6 text-white" />
            </div>
            <div className="flex-1">
              <div className="flex items-center space-x-2 mb-2">
                <h3 className="text-xl font-bold text-white">Step 3</h3>
                <span className="text-green-400 text-sm font-medium">Start Typing</span>
              </div>
              <p className="text-gray-400 mb-4">
                Begin typing on your device. The program automatically captures keystrokes and sends data every 5 minutes.
              </p>
              <div className="bg-green-500/10 border border-green-500/30 rounded-lg p-3">
                <div className="flex items-start space-x-2">
                  <CheckCircle className="w-5 h-5 text-green-400 flex-shrink-0 mt-0.5" />
                  <div>
                    <p className="text-green-400 text-sm font-medium">Automatic Sync</p>
                    <p className="text-gray-400 text-xs mt-1">
                      Data syncs every 5 minutes in the background. No manual action required!
                    </p>
                  </div>
                </div>
              </div>
            </div>
          </div>
          
          {/* Step 3 Image */}
          <div className="mt-6 flex justify-center">
            <div className="rounded-lg overflow-hidden border border-gray-600/30 max-w-xl">
              <img 
                src="/step 3.png" 
                alt="Running program showing system tray icon with active status"
                className="w-full h-auto"
              />
            </div>
          </div>
        </div>

        {/* Step 4: View Data */}
        <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-orange-500/30 rounded-xl p-6 hover:border-orange-500/50 transition-all duration-300">
          <div className="flex items-start space-x-4">
            <div className="w-12 h-12 bg-gradient-to-r from-orange-500 to-red-500 rounded-lg flex items-center justify-center flex-shrink-0">
              <CheckCircle className="w-6 h-6 text-white" />
            </div>
            <div className="flex-1">
              <div className="flex items-center space-x-2 mb-2">
                <h3 className="text-xl font-bold text-white">Step 4</h3>
                <span className="text-orange-400 text-sm font-medium">View Your Data</span>
              </div>
              <p className="text-gray-400 mb-4">
                Access your keystroke data from the dashboard. View in Overview or navigate to Devices section for detailed analysis.
              </p>
              <div className="space-y-2 text-sm">
                <div className="flex items-center space-x-2 text-gray-300">
                  <div className="w-2 h-2 bg-cyan-400 rounded-full"></div>
                  <span>Real-time device status</span>
                </div>
                <div className="flex items-center space-x-2 text-gray-300">
                  <div className="w-2 h-2 bg-purple-400 rounded-full"></div>
                  <span>Keystroke sequences by date</span>
                </div>
                <div className="flex items-center space-x-2 text-gray-300">
                  <div className="w-2 h-2 bg-green-400 rounded-full"></div>
                  <span>Activity statistics & charts</span>
                </div>
              </div>
            </div>
          </div>
          
          {/* Step 4 Image */}
          <div className="mt-6 flex justify-center">
            <div className="rounded-lg overflow-hidden border border-gray-600/30 max-w-2xl">
              <img 
                src="/step 4.png" 
                alt="Dashboard view showing device data and keystroke sequences"
                className="w-full h-auto"
              />
            </div>
          </div>
        </div>
      </div>

      {/* Additional Info */}
      <div className="neon-card bg-gradient-to-r from-cyan-500/10 to-purple-500/10 border border-cyan-500/30 rounded-xl p-6">
        <h3 className="text-lg font-bold text-white mb-3">Important Notes</h3>
        <ul className="space-y-2 text-gray-300 text-sm">
          <li className="flex items-start space-x-2">
            <CheckCircle className="w-5 h-5 text-cyan-400 flex-shrink-0 mt-0.5" />
            <span>The program runs quietly in the background and doesn't affect system performance</span>
          </li>
          <li className="flex items-start space-x-2">
            <CheckCircle className="w-5 h-5 text-cyan-400 flex-shrink-0 mt-0.5" />
            <span>All data is encrypted during transmission and storage</span>
          </li>
          <li className="flex items-start space-x-2">
            <CheckCircle className="w-5 h-5 text-cyan-400 flex-shrink-0 mt-0.5" />
            <span>You can add multiple devices to the same account</span>
          </li>
          <li className="flex items-start space-x-2">
            <CheckCircle className="w-5 h-5 text-cyan-400 flex-shrink-0 mt-0.5" />
            <span>Device appears in your dashboard immediately after first sync (max 5 minutes)</span>
          </li>
        </ul>
      </div>
    </div>
  );
};

export default AddDeviceGuide;
