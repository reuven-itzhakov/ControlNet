import React, { useState, useEffect } from 'react';
import { useParams, Link } from 'react-router-dom';
import { ArrowLeft, Monitor, Activity, Clock, Calendar, Keyboard, Filter, Download, Settings } from 'lucide-react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, BarChart, Bar } from 'recharts';
import { deviceAPI, sequenceAPI } from '../../services/api';
import { useAuth } from '../../context/AuthContext';

const DeviceDetail = () => {
  const { deviceId } = useParams();
  const { user } = useAuth();
  const [device, setDevice] = useState(null);
  const [keySequences, setKeySequences] = useState([]);
  const [activityData, setActivityData] = useState([]);
  const [selectedDate, setSelectedDate] = useState(new Date().toISOString().split('T')[0]);
  const [filterType, setFilterType] = useState('all');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchDeviceData = async () => {
      if (!user || !deviceId) return;
      
      try {
        setLoading(true);
        setError(null);
        
        // Fetch device details
        const deviceResult = await deviceAPI.getDevice(deviceId);
        if (deviceResult.success) {
          setDevice(deviceResult.device);
        }
        
        // Fetch sequences for the selected date
        const sequencesResult = await sequenceAPI.getSequences(deviceId, selectedDate);
        
        if (sequencesResult.success && sequencesResult.sequences) {
          // Process sequences to match expected format
          const processedSequences = sequencesResult.sequences.map((seq, index) => ({
            id: seq.id || index + 1,
            sequence: seq.sequence,
            timestamp: seq.timestamp,
            length: seq.sequence ? seq.sequence.length : 0,
            type: seq.type || 'text',
            application: seq.application || 'Unknown'
          }));
          
          setKeySequences(processedSequences);
        
          // Generate activity data from sequences (group by hour)
          const hourlyActivity = {};
          processedSequences.forEach(seq => {
            const hour = new Date(seq.timestamp).getHours();
            const hourKey = `${hour.toString().padStart(2, '0')}:00`;
            if (!hourlyActivity[hourKey]) {
              hourlyActivity[hourKey] = { hour: hourKey, sequences: 0, totalLength: 0 };
            }
            hourlyActivity[hourKey].sequences++;
            hourlyActivity[hourKey].totalLength += seq.length;
          });
          
          // Convert to array and calculate averages
          const activityArray = Object.values(hourlyActivity).map(data => ({
            ...data,
            avgLength: data.sequences > 0 ? (data.totalLength / data.sequences).toFixed(1) : 0
          }));
          
          setActivityData(activityArray);
        }
        
      } catch (err) {
        console.error('Error fetching device data:', err);
        setError('Failed to load device data');
      } finally {
        setLoading(false);
      }
    };

    fetchDeviceData();
  }, [user, deviceId, selectedDate]);

  const filteredSequences = keySequences.filter(seq => {
    if (filterType === 'all') return true;
    return seq.type === filterType;
  });

  const getSequenceTypeColor = (type) => {
    switch (type) {
      case 'code': return 'bg-blue-500/20 border-blue-500/50 text-blue-300';
      case 'command': return 'bg-green-500/20 border-green-500/50 text-green-300';
      case 'password': return 'bg-red-500/20 border-red-500/50 text-red-300';
      case 'text': return 'bg-purple-500/20 border-purple-500/50 text-purple-300';
      default: return 'bg-gray-500/20 border-gray-500/50 text-gray-300';
    }
  };

  if (!device && loading) {
    return (
      <div className="p-6 flex items-center justify-center">
        <div className="neon-spinner"></div>
      </div>
    );
  }

  if (!device && !loading) {
    return (
      <div className="p-6">
        <div className="text-center">
          <Monitor className="w-16 h-16 text-gray-500 mx-auto mb-4" />
          <p className="text-gray-400 text-lg">Device not found</p>
          <Link to="/dashboard" className="text-cyan-400 hover:text-cyan-300 mt-2 inline-block">
            ← Back to Dashboard
          </Link>
        </div>
      </div>
    );
  }

  return (
    <div className="p-6 space-y-6">
      {/* Error Message */}
      {error && (
        <div className="bg-red-500/10 border border-red-500/30 rounded-lg p-4">
          <p className="text-red-400">Error: {error}</p>
        </div>
      )}

      {/* Header */}
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-4">
          <Link
            to="/dashboard"
            className="p-2 text-gray-400 hover:text-white hover:bg-gray-700/50 rounded-lg transition-all"
          >
            <ArrowLeft className="w-5 h-5" />
          </Link>
          <div className="flex items-center space-x-3">
            <div className="w-12 h-12 bg-gradient-to-r from-cyan-500/20 to-purple-500/20 rounded-lg flex items-center justify-center">
              <Monitor className="w-6 h-6 text-cyan-400" />
            </div>
            <div>
              <h1 className="text-2xl font-bold text-white">{device.name}</h1>
              <p className="text-gray-400">{device.description}</p>
              <div className="flex items-center space-x-4 mt-1">
                <span className={`text-sm px-2 py-1 rounded ${device.status === 'active' ? 'bg-green-500/20 text-green-300' : 'bg-gray-500/20 text-gray-300'}`}>
                  {device.status}
                </span>
                {device.batteryLevel && (
                  <span className="text-sm text-green-400">{device.batteryLevel}% battery</span>
                )}
              </div>
            </div>
          </div>
        </div>
        <div className="flex items-center space-x-2">
          <button className="p-2 text-gray-400 hover:text-white hover:bg-gray-700/50 rounded-lg transition-all">
            <Settings className="w-5 h-5" />
          </button>
          <button className="p-2 text-gray-400 hover:text-white hover:bg-gray-700/50 rounded-lg transition-all">
            <Download className="w-5 h-5" />
          </button>
        </div>
      </div>

      {/* Stats Cards */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
        <div className="neon-card bg-gray-700/30 rounded-lg p-4">
          <div className="flex items-center space-x-2">
            <Keyboard className="w-5 h-5 text-cyan-400" />
            <span className="text-gray-300">Total Sequences</span>
          </div>
          <p className="text-2xl font-bold text-cyan-400 mt-1">{keySequences.length}</p>
        </div>
        <div className="neon-card bg-gray-700/30 rounded-lg p-4">
          <div className="flex items-center space-x-2">
            <Activity className="w-5 h-5 text-purple-400" />
            <span className="text-gray-300">Avg Length</span>
          </div>
          <p className="text-2xl font-bold text-purple-400 mt-1">
            {Math.round(keySequences.reduce((acc, seq) => acc + seq.length, 0) / keySequences.length || 0)}
          </p>
        </div>
        <div className="neon-card bg-gray-700/30 rounded-lg p-4">
          <div className="flex items-center space-x-2">
            <Clock className="w-5 h-5 text-green-400" />
            <span className="text-gray-300">Last Activity</span>
          </div>
          <p className="text-lg font-semibold text-green-400 mt-1">
            {device.lastActive?.seconds ? 
              new Date(device.lastActive.seconds * 1000).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }) :
              'N/A'
            }
          </p>
        </div>
        <div className="neon-card bg-gray-700/30 rounded-lg p-4">
          <div className="flex items-center space-x-2">
            <Monitor className="w-5 h-5 text-blue-400" />
            <span className="text-gray-300">Applications</span>
          </div>
          <p className="text-2xl font-bold text-blue-400 mt-1">
            {new Set(keySequences.map(seq => seq.application)).size}
          </p>
        </div>
      </div>

      {/* Activity Chart */}
      {activityData.length > 0 ? (
        <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6">
          <h2 className="text-xl font-bold text-white mb-4">Daily Activity Pattern</h2>
          <div className="h-64">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={activityData}>
                <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                <XAxis dataKey="hour" stroke="#9CA3AF" />
                <YAxis stroke="#9CA3AF" />
                <Tooltip 
                  contentStyle={{ 
                    backgroundColor: '#1F2937', 
                    border: '1px solid #374151', 
                    borderRadius: '8px',
                    color: '#F3F4F6'
                  }}
                />
                <Bar dataKey="sequences" fill="#06B6D4" radius={[4, 4, 0, 0]} />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>
      ) : (
        <div className="neon-card bg-gray-800/50 rounded-xl p-12 text-center">
          <Activity className="w-16 h-16 text-gray-500 mx-auto mb-4" />
          <p className="text-gray-400 text-lg">No activity data for selected date</p>
        </div>
      )}

      {/* Keystroke Sequence Log */}
      <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6">
        <div className="flex items-center justify-between mb-6">
          <h3 className="text-lg font-semibold text-white">Keystroke Sequences</h3>
          <div className="flex items-center space-x-4">
            <div className="flex items-center space-x-2">
              <Calendar className="w-5 h-5 text-gray-400" />
              <input
                type="date"
                value={selectedDate}
                onChange={(e) => setSelectedDate(e.target.value)}
                className="neon-input px-3 py-2 bg-gray-700/50 border border-gray-600 rounded-lg text-white focus:border-cyan-400 focus:ring-1 focus:ring-cyan-400 transition-all"
              />
            </div>
            <div className="flex items-center space-x-2">
              <Filter className="w-5 h-5 text-gray-400" />
              <select
                value={filterType}
                onChange={(e) => setFilterType(e.target.value)}
                className="neon-input px-3 py-2 bg-gray-700/50 border border-gray-600 rounded-lg text-white focus:border-cyan-400 focus:ring-1 focus:ring-cyan-400 transition-all"
              >
                <option value="all">All Types</option>
                <option value="code">Code</option>
                <option value="command">Commands</option>
                <option value="text">Text</option>
                <option value="password">Passwords</option>
              </select>
            </div>
          </div>
        </div>
        
        {loading ? (
          <div className="flex items-center justify-center py-8">
            <div className="neon-spinner"></div>
          </div>
        ) : filteredSequences.length === 0 ? (
          <div className="text-center py-8">
            <Keyboard className="w-12 h-12 text-gray-500 mx-auto mb-4" />
            <p className="text-gray-400">No keystroke sequences found for the selected criteria</p>
          </div>
        ) : (
          <>
            <div className="space-y-3 max-h-96 overflow-y-auto">
              {filteredSequences.map((sequence) => (
                <div
                  key={sequence.id}
                  className="flex items-center justify-between p-4 bg-gray-700/30 rounded-lg border border-gray-600/30 hover:border-cyan-500/30 transition-all"
                >
                  <div className="flex items-center space-x-4">
                    <div className="w-2 h-2 bg-cyan-400 rounded-full"></div>
                    <div>
                      <div className="flex items-center space-x-2 mb-1">
                        <p className="text-white font-mono text-sm bg-gray-600/50 px-3 py-1 rounded">
                          {sequence.type === 'password' ? '••••••••••' : sequence.sequence}
                        </p>
                        <span className={`text-xs px-2 py-1 rounded border ${getSequenceTypeColor(sequence.type)}`}>
                          {sequence.type}
                        </span>
                      </div>
                      <div className="flex items-center space-x-4 text-xs text-gray-400">
                        <span>App: {sequence.application}</span>
                        <span>Length: {sequence.length} keys</span>
                      </div>
                    </div>
                  </div>
                  <div className="text-right">
                    <p className="text-gray-300 text-sm">
                      {new Date(sequence.timestamp).toLocaleTimeString([], { 
                        hour: '2-digit', 
                        minute: '2-digit', 
                        second: '2-digit' 
                      })}
                    </p>
                    <p className="text-gray-500 text-xs">
                      {new Date(sequence.timestamp).toLocaleDateString()}
                    </p>
                  </div>
                </div>
              ))}
            </div>
            
            <div className="mt-6 pt-4 border-t border-gray-600/30">
              <div className="grid grid-cols-1 md:grid-cols-3 gap-4 text-sm">
                <div>
                  <span className="text-gray-400">Total sequences today: </span>
                  <span className="text-cyan-400 font-semibold">{filteredSequences.length}</span>
                </div>
                <div>
                  <span className="text-gray-400">Average length: </span>
                  <span className="text-purple-400 font-semibold">
                    {Math.round(filteredSequences.reduce((acc, seq) => acc + seq.length, 0) / filteredSequences.length || 0)} keys
                  </span>
                </div>
                <div>
                  <span className="text-gray-400">Most active hour: </span>
                  <span className="text-green-400 font-semibold">
                    {activityData.length > 0 ? 
                      activityData.reduce((max, data) => data.sequences > max.sequences ? data : max, activityData[0]).hour :
                      'N/A'
                    }
                  </span>
                </div>
              </div>
            </div>
          </>
        )}
      </div>
    </div>
  );
};

export default DeviceDetail;