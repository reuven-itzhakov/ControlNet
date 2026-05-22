import React from 'react';
import { Keyboard } from 'lucide-react';

const RecentSequences = ({ recentSequences = [] }) => {
  // Function to get consistent colors for app names (same as KeystrokeSequence)
  const getAppColor = (appName) => {
    if (!appName) return 'bg-gray-500/20 border-gray-500/50 text-gray-400';
    
    const colors = [
      'bg-blue-500/20 border-blue-500/50 text-blue-400',
      'bg-green-500/20 border-green-500/50 text-green-400',
      'bg-purple-500/20 border-purple-500/50 text-purple-400',
      'bg-pink-500/20 border-pink-500/50 text-pink-400',
      'bg-yellow-500/20 border-yellow-500/50 text-yellow-400',
      'bg-red-500/20 border-red-500/50 text-red-400',
      'bg-indigo-500/20 border-indigo-500/50 text-indigo-400',
      'bg-teal-500/20 border-teal-500/50 text-teal-400',
      'bg-orange-500/20 border-orange-500/50 text-orange-400',
      'bg-emerald-500/20 border-emerald-500/50 text-emerald-400',
      'bg-violet-500/20 border-violet-500/50 text-violet-400',
      'bg-rose-500/20 border-rose-500/50 text-rose-400'
    ];
    
    // Create a simple hash from app name to get consistent color
    let hash = 0;
    for (let i = 0; i < appName.length; i++) {
      hash = ((hash << 5) - hash + appName.charCodeAt(i)) & 0xffffffff;
    }
    
    return colors[Math.abs(hash) % colors.length];
  };

  return (
    <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6">
      <h2 className="text-xl font-bold text-white mb-4">Recent Keystroke Sequences</h2>
      <div className="space-y-3 max-h-[400px] overflow-y-auto custom-scrollbar">
        {recentSequences.length === 0 ? (
          <div className="flex flex-col items-center justify-center py-8">
            <Keyboard className="w-16 h-16 text-gray-600 mb-4" />
            <p className="text-gray-400 text-lg font-medium">No Recent Sequences</p>
            <p className="text-gray-500 text-sm mt-2">Recent keystroke activity will appear here</p>
          </div>
        ) : (
          recentSequences.map((item, index) => (
          <div key={index} className="p-3 bg-gray-700/30 rounded-lg border border-gray-600/30 hover:border-cyan-500/30 transition-all">
            {/* Header: Device, App, Time */}
            <div className="flex items-center justify-between mb-2">
              <div className="flex items-center space-x-2">
                <div className="w-2 h-2 bg-cyan-400 rounded-full"></div>
                <span className="text-gray-400 text-xs">{item.device}</span>
                {item.appName && (
                  <>
                    <span className="text-gray-600">•</span>
                    <span className={`text-xs px-2 py-0.5 rounded border ${getAppColor(item.appName)}`}>
                      {item.appName}
                    </span>
                  </>
                )}
              </div>
              <div className="text-right">
                <p className="text-gray-400 text-xs">{item.time}</p>
              </div>
            </div>
            
            {/* Sequence Text */}
            <div className="flex items-center justify-between">
              <p className="text-white font-mono text-sm bg-gray-700/50 px-2 py-1 rounded flex-1 mr-2 break-words">
                "{item.sequence}"
              </p>
              <p className="text-purple-400 text-xs whitespace-nowrap">{item.length} chars</p>
            </div>
          </div>
          ))
        )}
      </div>
    </div>
  );
};

export default RecentSequences;