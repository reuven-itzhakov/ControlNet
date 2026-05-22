import React from 'react';
import { Keyboard, Clock, Activity } from 'lucide-react';

const SequenceStats = ({ sequenceStats }) => (
  <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-6">
    <div className="neon-card bg-gray-700/30 rounded-lg p-4">
      <div className="flex items-center space-x-2">
        <Keyboard className="w-5 h-5 text-cyan-400" />
        <span className="text-gray-300">Total Keystrokes</span>
      </div>
      <p className="text-2xl font-bold text-cyan-400 mt-1">{sequenceStats.totalKeys}</p>
    </div>
    <div className="neon-card bg-gray-700/30 rounded-lg p-4">
      <div className="flex items-center space-x-2">
        <Clock className="w-5 h-5 text-purple-400" />
        <span className="text-gray-300">Started</span>
      </div>
      <p className="text-lg font-semibold text-purple-400 mt-1">
        {sequenceStats.startTime ? new Date(sequenceStats.startTime).toLocaleTimeString() : '--'}
      </p>
    </div>
    <div className="neon-card bg-gray-700/30 rounded-lg p-4">
      <div className="flex items-center space-x-2">
        <Activity className="w-5 h-5 text-green-400" />
        <span className="text-gray-300">Last Key</span>
      </div>
      <p className="text-lg font-semibold text-green-400 mt-1">
        {sequenceStats.endTime ? new Date(sequenceStats.endTime).toLocaleTimeString() : '--'}
      </p>
    </div>
  </div>
);

export default SequenceStats;