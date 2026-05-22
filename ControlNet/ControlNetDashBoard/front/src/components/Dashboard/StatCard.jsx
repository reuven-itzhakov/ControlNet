import React from 'react';

const StatCard = ({ title, value, subtitle, icon: Icon, color = 'cyan' }) => (
  <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6">
    <div className="flex items-center justify-between">
      <div>
        <p className="text-gray-400 text-sm">{title}</p>
        <p className={`text-2xl font-bold text-${color}-400 mt-1`}>{value}</p>
        {subtitle && <p className="text-gray-500 text-xs mt-1">{subtitle}</p>}
      </div>
      <div className={`w-12 h-12 bg-gradient-to-r from-${color}-500/20 to-purple-500/20 rounded-lg flex items-center justify-center`}>
        <Icon className={`w-6 h-6 text-${color}-400`} />
      </div>
    </div>
  </div>
);

export default StatCard;