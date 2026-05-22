import React, { useState, useEffect, useRef } from 'react';
import { useAuth } from '../../context/AuthContext';
import { Monitor, Activity, Clock, TrendingUp, Keyboard, Users } from 'lucide-react';
import { deviceAPI, sequenceAPI } from '../../services/api';
import StatCard from './StatCard';
import OverviewDeviceCard from './OverviewDeviceCard';
import ActivityChart from './ActivityChart';
import RecentSequences from './RecentSequences';
import AddDeviceGuide from './AddDeviceGuide';

const Overview = () => {
  const { user } = useAuth();
  const guideRef = useRef(null);
  const [sequenceData, setSequenceData] = useState([]);
  const [recentSequences, setRecentSequences] = useState([]);
  const [devices, setDevices] = useState([]);
  const [stats, setStats] = useState({
    totalSequences: 0,
    todaySequences: 0,
    activeDevices: 0
  });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  // Smooth scroll to add device guide
  const scrollToGuide = () => {
    guideRef.current?.scrollIntoView({ 
      behavior: 'smooth', 
      block: 'start' 
    });
  };

  useEffect(() => {
    const fetchData = async () => {
      if (!user) return;
      
      setLoading(true);
      setError('');
      
      try {
        // Fetch user devices
        const devicesResult = await deviceAPI.getDevices(user.email);
        if (!devicesResult.success) {
          throw new Error(devicesResult.error);
        }
        
        setDevices(devicesResult.devices || []);
        
        // Get today's date in YYYY-MM-DD format
        const today = new Date().toISOString().split('T')[0];
        
        // Calculate total sequences by summing from all devices
        const totalSequences = devicesResult.devices.reduce((sum, device) => {
          return sum + (device.totalSequences || 0);
        }, 0);
        
        // Calculate today's sequences - only count if todayDate matches today
        const todaySequences = devicesResult.devices.reduce((sum, device) => {
          // Check if device's todayDate matches today's date
          if (device.todayDate === today) {
            return sum + (device.todaySequences || 0);
          }
          return sum;
        }, 0);
        
        // Calculate stats
        const activeDevices = devicesResult.devices.filter(device => {
          return new Date(device.lastUpdated) > new Date(Date.now() - 24 * 60 * 60 * 1000);
        }).length;
        
        setStats({
          totalSequences: totalSequences,
          todaySequences: todaySequences,
          activeDevices
        });

        // Fetch today's activity data for chart
        const hourlyData = Array(24).fill(0); // Initialize 24 hours with 0
        const allSequences = []; // Collect all sequences for recent display
        
        // Fetch keystrokes for all devices for today
        for (const device of devicesResult.devices) {
          try {
            const result = await sequenceAPI.getSequences(device.id, today);
            
            if (result.success && result.sequences) {
              // Count keystrokes per hour
              result.sequences.forEach(keystroke => {
                const hour = new Date(keystroke.timestamp).getHours();
                hourlyData[hour]++;
                
                // Add device info for recent sequences
                allSequences.push({
                  ...keystroke,
                  deviceId: device.id,
                  deviceName: device.computerName || device.id
                });
              });
            }
          } catch (err) {
            console.error(`Error fetching keystrokes for device ${device.id}:`, err);
          }
        }
        
        // Format data for chart
        const chartData = hourlyData.map((count, hour) => ({
          time: `${hour.toString().padStart(2, '0')}:00`,
          sequences: count
        }));
        
        setSequenceData(chartData);
        
        // Get recent sequences (last 10, sorted by timestamp)
        const sortedSequences = allSequences.sort((a, b) => 
          new Date(b.timestamp) - new Date(a.timestamp)
        );
        
        const recentTop10 = sortedSequences.slice(0, 10).map(seq => {
          const timestamp = new Date(seq.timestamp);
          const now = new Date();
          const diffMinutes = Math.floor((now - timestamp) / 1000 / 60);
          
          let timeAgo;
          if (diffMinutes < 1) {
            timeAgo = 'Just now';
          } else if (diffMinutes < 60) {
            timeAgo = `${diffMinutes} min ago`;
          } else {
            const hours = Math.floor(diffMinutes / 60);
            timeAgo = `${hours}h ago`;
          }
          
          return {
            sequence: seq.text.length > 50 ? seq.text.substring(0, 50) + '...' : seq.text,
            device: seq.deviceName,
            time: timeAgo,
            length: seq.length,
            appName: seq.appName
          };
        });
        
        setRecentSequences(recentTop10);
        
      } catch (error) {
        console.error('Error fetching overview data:', error);
        setError(error.message);
      } finally {
        setLoading(false);
      }
    };

    fetchData();
  }, [user]);

  return (
    <div className="p-6 space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-white mb-2">
          Welcome back, {user?.name}!
        </h1>
        <p className="text-gray-400">Here's your keystroke sequence overview</p>
      </div>

      {/* Error Message */}
      {error && (
        <div className="bg-red-500/10 border border-red-500/30 rounded-lg p-4">
          <p className="text-red-400">Error: {error}</p>
        </div>
      )}

      {/* Loading State */}
      {loading ? (
        <div className="flex items-center justify-center py-12">
          <div className="neon-spinner"></div>
        </div>
      ) : (
        <>
          {/* Stats Grid */}
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
            <StatCard
              title="Total Sequences"
              value={stats.totalSequences.toLocaleString()}
              subtitle="All time"
              icon={Keyboard}
              color="cyan"
            />
            <StatCard
              title="Today's Sequences"
              value={stats.todaySequences.toLocaleString()}
              subtitle="Since midnight"
              icon={Activity}
              color="green"
            />
            <StatCard
              title="Active Devices"
              value={stats.activeDevices}
              subtitle={`${devices.length} total devices`}
              icon={Monitor}
              color="purple"
            />
          </div>

          {/* Charts Section */}
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
            {/* Sequence Activity Chart */}
            <ActivityChart sequenceData={sequenceData} />

            {/* Recent Sequences */}
            <RecentSequences recentSequences={recentSequences} />
          </div>

          {/* Add Device Call to Action */}
          <div className="flex flex-col items-center justify-center py-2">
            <button 
              onClick={scrollToGuide}
              className="group relative px-8 py-4 bg-gradient-to-r from-cyan-500 to-purple-500 text-white text-lg font-bold rounded-xl hover:from-cyan-400 hover:to-purple-400 transition-all duration-300 shadow-lg hover:shadow-cyan-500/50 hover:scale-105"
            >
              <span className="flex items-center space-x-2">
                <Monitor className="w-6 h-6" />
                <span>Add Your First Device</span>
              </span>
            </button>
            
            {/* Animated Double Arrows */}
            <div className="mt-6 flex flex-col items-center animate-bounce">
              <svg 
                className="w-6 h-6 text-cyan-400 translate-y-1" 
                fill="none" 
                stroke="currentColor" 
                viewBox="0 0 24 24"
              >
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={3} d="M19 9l-7 7-7-7" />
              </svg>
              <svg 
                className="w-6 h-6 text-cyan-400 -translate-y-2" 
                fill="none" 
                stroke="currentColor" 
                viewBox="0 0 24 24"
              >
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={3} d="M19 9l-7 7-7-7" />
              </svg>
            </div>
            
            <p className="mt-4 text-gray-400 text-sm">Scroll down for step-by-step instructions</p>
          </div>

          {/* Add Device Guide Section */}
          <div ref={guideRef} className="pt-12">
            <AddDeviceGuide />
          </div>
        </>
      )}
    </div>
  );
};

export default Overview;