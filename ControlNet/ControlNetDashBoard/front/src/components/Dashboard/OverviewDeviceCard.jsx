import React from 'react';
import { Link } from 'react-router-dom';
import { Monitor } from 'lucide-react';

const OverviewDeviceCard = ({ device }) => {
  const getDeviceStatus = () => {
    if (!device.lastActive) return 'inactive';
    
    const lastActive = new Date(device.lastActive?.seconds * 1000 || device.lastActive);
    const hourAgo = new Date(Date.now() - 60 * 60 * 1000);
    return lastActive > hourAgo ? 'active' : 'inactive';
  };

  const getLastActiveTime = () => {
    if (!device.lastActive) return 'Never';
    
    const lastActive = new Date(device.lastActive?.seconds * 1000 || device.lastActive);
    return lastActive.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  };

  return (
    <Link
      to={`/dashboard/device/${device.id}`}
      className="neon-card bg-gray-800/50 backdrop-blur-sm border border-gray-600/30 hover:border-cyan-500/50 rounded-xl p-6 transition-all group"
    >
      <div className="flex items-center justify-between mb-4">
        <div className="flex items-center space-x-3">
          <div className="w-10 h-10 bg-gradient-to-r from-cyan-500/20 to-purple-500/20 rounded-lg flex items-center justify-center">
            <Monitor className="w-5 h-5 text-cyan-400" />
          </div>
          <div>
            <h3 className="text-white font-medium group-hover:text-cyan-400 transition-colors">
              {device.name}
            </h3>
            <p className="text-gray-400 text-sm">{device.type || 'device'}</p>
          </div>
        </div>
        <div className={`w-3 h-3 rounded-full ${getDeviceStatus() === 'active' ? 'bg-green-400 neon-glow-sm' : 'bg-gray-500'}`}></div>
      </div>
      
      <div className="space-y-3">
        <div className="flex justify-between text-sm">
          <span className="text-gray-400">Last Active</span>
          <span className="text-gray-300">
            {getLastActiveTime()}
          </span>
        </div>
        <div className="flex justify-between text-sm">
          <span className="text-gray-400">Status</span>
          <span className={`font-medium ${getDeviceStatus() === 'active' ? 'text-green-400' : 'text-gray-400'}`}>
            {getDeviceStatus()}
          </span>
        </div>
      </div>
    </Link>
  );
};

export default OverviewDeviceCard;