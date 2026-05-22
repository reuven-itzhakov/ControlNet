import React from 'react';
import { Keyboard } from 'lucide-react';
import SequenceStats from './SequenceStats';

const KeystrokeSequence = ({ keySequence, sequenceStats }) => {
  // Function to get consistent colors for app names
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
  if (!keySequence.length) {
    return (
      <div className="text-center py-8">
        <Keyboard className="w-12 h-12 text-gray-500 mx-auto mb-4" />
        <p className="text-gray-400">No keystroke data for selected date</p>
      </div>
    );
  }

  return (
    <div className="space-y-4">
      {/* Sequence Stats */}
      <SequenceStats sequenceStats={sequenceStats} />

      {/* Keystroke Sequence Display */}
      <div className="neon-card bg-gray-800/50 rounded-xl p-6 overflow-y-auto">
        <h4 className="text-lg font-semibold text-white mb-4">Keystroke Sequence</h4>
        <div className="space-y-2">
          {keySequence.map((keystroke, index) => (
            <div
              key={keystroke.id}
              className="p-3 bg-gray-700/30 rounded-lg border border-gray-600/30 hover:border-cyan-500/30 transition-all"
            >
              {/* Header Row: Index, Time, App Name */}
              <div className="flex items-center justify-between mb-2">
                <div className="flex items-center space-x-3">
                  <span className="text-gray-500 text-sm font-medium">{index + 1}</span>
                  
                  {/* Time display in HH:MM:SS format */}
                  <span className="text-cyan-400 font-mono text-sm">
                    {new Date(keystroke.timestamp).toLocaleTimeString([], { 
                      hour: '2-digit', 
                      minute: '2-digit', 
                      second: '2-digit'
                    })}
                  </span>
                  
                  {/* App name badge */}
                  {keystroke.appName && (
                    <span className={`text-xs px-2 py-1 rounded border whitespace-nowrap ${getAppColor(keystroke.appName)}`}>
                      {keystroke.appName}
                    </span>
                  )}
                </div>
                
                {/* Text length indicator */}
                <span className="text-purple-400 text-xs whitespace-nowrap">
                  {keystroke.length} chars
                </span>
              </div>
              
              {/* Text Content - Full Width */}
              <div className="bg-gray-600/50 px-3 py-2 rounded border border-gray-500/50 w-full">
                <span className="text-white font-mono text-sm break-words">"{keystroke.text}"</span>
              </div>
              
              {/* Original Timestamp - Hidden on mobile, shown on desktop */}
              <div className="hidden sm:block text-right mt-2">
                <p className="text-gray-500 text-xs font-mono">
                  {keystroke.originalTimestamp}
                </p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default KeystrokeSequence;