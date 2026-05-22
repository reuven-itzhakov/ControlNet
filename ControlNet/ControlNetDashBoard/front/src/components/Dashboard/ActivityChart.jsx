import React from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, ResponsiveContainer } from 'recharts';
import { TrendingUp } from 'lucide-react';

const ActivityChart = ({ sequenceData }) => {
  return (
    <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6 h-full flex flex-col">
      <h2 className="text-xl font-bold text-white mb-4">Today's Sequence Activity</h2>
      <div className="flex-1 min-h-[300px]">
        {!sequenceData || sequenceData.length === 0 ? (
          <div className="flex flex-col items-center justify-center h-full">
            <TrendingUp className="w-16 h-16 text-gray-600 mb-4" />
            <p className="text-gray-400 text-lg font-medium">No Activity Data</p>
            <p className="text-gray-500 text-sm mt-2">No sequence data available for today</p>
          </div>
        ) : (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={sequenceData} margin={{ top: 5, right: 20, left: 0, bottom: 0 }}>
              <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
              <XAxis dataKey="time" stroke="#9CA3AF" />
              <YAxis stroke="#9CA3AF" />
              <Line 
                type="monotone" 
                dataKey="sequences" 
                stroke="#06B6D4" 
                strokeWidth={2}
                dot={{ fill: '#06B6D4', strokeWidth: 2, r: 4 }}
                name="Sequences"
              />
            </LineChart>
          </ResponsiveContainer>
        )}
      </div>
    </div>
  );
};

export default ActivityChart;