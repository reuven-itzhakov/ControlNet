import React, { useState } from 'react';
import { Monitor, ChevronRight, Info, X } from 'lucide-react';

const DeviceCard = ({ device, isSelected, onClick }) => {
  const [showInfo, setShowInfo] = useState(false);

  return (
  <>
  <div
    onClick={() => onClick(device.id)}
    className={`neon-card cursor-pointer transition-all ${
      isSelected 
        ? 'bg-gradient-to-r from-cyan-500/20 to-purple-500/20 border-cyan-500/50 neon-glow-sm' 
        : 'bg-gray-800/50 border-gray-600/30 hover:border-cyan-500/50'
    } backdrop-blur-sm rounded-xl p-6`}
  >
    <div className="flex items-center justify-between mb-4">
      <div className="flex items-center space-x-3">
        <div className="relative w-12 h-12 rounded-lg flex items-center justify-center">
          <div className={`absolute inset-0 rounded-lg bg-gradient-to-r from-cyan-500/20 to-purple-500/20 transition-opacity duration-500
           ${isSelected ? 'opacity-0' : 'opacity-100'}`}>
          </div>
          <div className={`absolute inset-0 rounded-lg bg-gradient-to-r from-cyan-400 to-purple-400 neon-glow-sm transition-opacity duration-500
           ${isSelected ? 'opacity-100' : 'opacity-0'}`}>
          </div>
          <Monitor className={`w-6 h-6 relative z-10 transition-colors duration-500 ease-in-out
           ${isSelected ? 'text-gray-900' : 'text-cyan-400'}`} />
        </div>
        <div>
          <h3 className={`font-medium transition-colors ${
            isSelected ? 'text-cyan-400' : 'text-white hover:text-cyan-400'
          }`}>
            {device.computerName || device.id}
          </h3>
          <p className="text-gray-400 text-sm">{device.userName || 'Unknown User'}</p>
        </div>
      </div>
      <div className="flex items-center space-x-2">
        <div className="flex items-center space-x-1.5">
          <div className={`w-3 h-3 rounded-full transition-colors duration-300 ${
            device.lastUpdated && new Date(device.lastUpdated) > new Date(Date.now() - 24*60*60*1000) 
              ? 'bg-green-400 neon-glow-sm' 
              : 'bg-gray-500'
          }`}></div>
          <span className={`text-xs font-medium transition-colors duration-300 ${
            device.lastUpdated && new Date(device.lastUpdated) > new Date(Date.now() - 24*60*60*1000) 
              ? 'text-green-400' 
              : 'text-gray-500'
          }`}>
            {device.lastUpdated && new Date(device.lastUpdated) > new Date(Date.now() - 24*60*60*1000) 
              ? 'Online' 
              : 'Offline'}
          </span>
        </div>
        <button
          onClick={(e) => {
            e.stopPropagation();
            setShowInfo(true);
          }}
          className="p-1.5 bg-gray-700/50 hover:bg-cyan-500/20 rounded-lg transition-all duration-300 group"
          title="Device Information"
        >
          <Info className="w-4 h-4 text-gray-400 group-hover:text-cyan-400 transition-colors duration-300" />
        </button>
        <ChevronRight className={`w-5 h-5 text-cyan-400 transition-opacity duration-300 ease-in-out ${
          isSelected ? 'opacity-100' : 'opacity-0'
        }`} />
      </div>
    </div>
    
    <div className="space-y-2 text-sm">
      <div className="flex justify-between">
        <span className="text-gray-400">Last Updated</span>
        <span className="text-gray-300">
          {device.lastUpdated 
            ? new Date(device.lastUpdated).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
            : 'Never'
          }
        </span>
      </div>
    </div>
  </div>

  {/* Device Info Modal */}
  {showInfo && (
    <div 
      className="fixed inset-0 bg-black/60 backdrop-blur-sm flex items-center justify-center z-50 p-4"
      onClick={() => setShowInfo(false)}
    >
      <div 
        className="neon-card bg-gray-800 border border-cyan-500/50 rounded-xl p-6 max-w-2xl w-full max-h-[80vh] overflow-y-auto"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Modal Header */}
        <div className="flex items-center justify-between mb-6">
          <div className="flex items-center space-x-3">
            <div className="w-10 h-10 bg-gradient-to-r from-cyan-400 to-purple-400 rounded-lg flex items-center justify-center">
              <Monitor className="w-6 h-6 text-gray-900" />
            </div>
            <div>
              <h3 className="text-xl font-bold text-white">{device.computerName || device.id}</h3>
              <p className="text-gray-400 text-sm">{device.userName || 'Unknown User'}</p>
            </div>
          </div>
          <button
            onClick={() => setShowInfo(false)}
            className="p-2 bg-gray-700/50 hover:bg-red-500/20 rounded-lg transition-all duration-300 group"
          >
            <X className="w-5 h-5 text-gray-400 group-hover:text-red-400 transition-colors duration-300" />
          </button>
        </div>

        {/* Device Information Grid */}
        <div className="space-y-4">
          {/* Statistics */}
          <div>
            <h4 className="text-cyan-400 font-semibold mb-3 flex items-center">
              <span className="w-1 h-4 bg-cyan-400 rounded-full mr-2"></span>
              Statistics
            </h4>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
              <InfoItem 
                label="Total Sequences" 
                value={(device.totalSequences || 0).toLocaleString()} 
              />
              <InfoItem 
                label="Today's Sequences" 
                value={(device.todaySequences || 0).toLocaleString()} 
              />
            </div>
          </div>

          {/* System Information */}
          <div>
            <h4 className="text-purple-400 font-semibold mb-3 flex items-center">
              <span className="w-1 h-4 bg-purple-400 rounded-full mr-2"></span>
              System Information
            </h4>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
              <InfoItem label="Computer Name" value={device.computerName || 'N/A'} />
              <InfoItem label="User Name" value={device.userName || 'N/A'} />
              <InfoItem label="Windows Version" value={device.windowsVersion || 'N/A'} />
              <InfoItem label="Architecture" value={device.architecture || 'N/A'} />
              <InfoItem label="System Uptime" value={device.systemUptime || 'N/A'} />
            </div>
          </div>

          {/* Hardware Information */}
          <div>
            <h4 className="text-pink-400 font-semibold mb-3 flex items-center">
              <span className="w-1 h-4 bg-pink-400 rounded-full mr-2"></span>
              Hardware Specifications
            </h4>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
              <InfoItem label="CPU Name" value={device.cpuName || 'N/A'} fullWidth />
              <InfoItem label="CPU Cores" value={device.cpuCores || 'N/A'} />
              <InfoItem label="CPU Speed" value={device.cpuSpeed || 'N/A'} />
              <InfoItem label="Total Memory" value={device.totalMemory || 'N/A'} />
              <InfoItem label="Available Memory" value={device.availableMemory || 'N/A'} />
            </div>
          </div>

          {/* Network Information */}
          <div>
            <h4 className="text-green-400 font-semibold mb-3 flex items-center">
              <span className="w-1 h-4 bg-green-400 rounded-full mr-2"></span>
              Network Information
            </h4>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
              <InfoItem label="Primary IP Address" value={device.primaryIPAddress || 'N/A'} />
              <InfoItem label="MAC Address" value={device.primaryMacAddress || 'N/A'} />
              {device.allIPAddresses && device.allIPAddresses.length > 0 && (
                <InfoItem 
                  label="All IP Addresses" 
                  value={device.allIPAddresses.join(', ')} 
                  fullWidth 
                />
              )}
            </div>
          </div>

          {/* Timestamps */}
          <div>
            <h4 className="text-orange-400 font-semibold mb-3 flex items-center">
              <span className="w-1 h-4 bg-orange-400 rounded-full mr-2"></span>
              Activity
            </h4>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
              <InfoItem 
                label="First Seen" 
                value={device.firstSeen ? new Date(device.firstSeen).toLocaleString() : 'N/A'} 
              />
              <InfoItem 
                label="Last Updated" 
                value={device.lastUpdated ? new Date(device.lastUpdated).toLocaleString() : 'N/A'} 
              />
            </div>
          </div>

          {/* Software */}
          {device.installedSoftware && device.installedSoftware.length > 0 && (
            <div>
              <h4 className="text-blue-400 font-semibold mb-3 flex items-center">
                <span className="w-1 h-4 bg-blue-400 rounded-full mr-2"></span>
                Installed Software ({device.installedSoftware.length})
              </h4>
              <div className="bg-gray-700/30 rounded-lg p-3 max-h-40 overflow-y-auto">
                <div className="space-y-1">
                  {device.installedSoftware.map((software, idx) => (
                    <div key={idx} className="text-gray-300 text-sm">• {software}</div>
                  ))}
                </div>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  )}
  </>
  );
};

// Helper component for displaying info items
const InfoItem = ({ label, value, fullWidth = false }) => (
  <div className={`bg-gray-700/30 rounded-lg p-3 ${fullWidth ? 'md:col-span-2' : ''}`}>
    <p className="text-gray-400 text-xs mb-1">{label}</p>
    <p className="text-white text-sm font-medium break-words">{value}</p>
  </div>
);

export default DeviceCard;