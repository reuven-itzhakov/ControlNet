import React, { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import { Monitor, Calendar, Keyboard, Camera } from 'lucide-react';
import { deviceAPI, sequenceAPI, screenshotAPI } from '../../services/api';
import { useAuth } from '../../context/AuthContext';
import DeviceCard from './DeviceCard';
import KeystrokeSequence from './KeystrokeSequence';
import CustomCalendar from './CustomCalendar';
import ScreenshotsGallery from './ScreenshotsGallery';

const DevicesList = () => {
  const { user } = useAuth();
  const [devices, setDevices] = useState([]);
  const [selectedDevice, setSelectedDevice] = useState(null);
  const [selectedDate, setSelectedDate] = useState(new Date().toISOString().split('T')[0]);
  const [activeTab, setActiveTab] = useState('keystrokes'); // 'keystrokes' or 'screenshots'
  const [keySequence, setKeySequence] = useState([]);
  const [screenshots, setScreenshots] = useState([]);
  const [sequenceStats, setSequenceStats] = useState({ totalKeys: 0, startTime: null, endTime: null });
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchDevices = async () => {
      if (!user) return;
      setLoading(true);
      try {
        setError(null);
        const result = await deviceAPI.getDevices(user.email);
        if (result.success) {
          setDevices(result.devices || []);
          if (result.devices && result.devices.length > 0) {
            setSelectedDevice(result.devices[0].id);
          }
        }
      } catch (err) {
        console.error('Error fetching devices:', err);
        setError('Failed to load devices');
      } finally {
        setLoading(false);
      }
    };

    fetchDevices();
  }, [user]);

  useEffect(() => {
    const fetchSequences = async () => {
      if (!selectedDevice || !selectedDate || !user) return;
      
      try {
        //setLoading(true);
        setError(null);
        
        // Get keystrokes for the selected device and date
        const result = await sequenceAPI.getSequences(selectedDevice, selectedDate);
        
        if (result.success && result.sequences) {
          // Add incremental IDs for display
          const keystrokesWithIds = result.sequences.map((keystroke, index) => ({
            ...keystroke,
            id: index + 1
          }));
          
          setKeySequence(keystrokesWithIds);
          
          if (keystrokesWithIds.length > 0) {
            setSequenceStats({
              totalKeys: keystrokesWithIds.length,
              startTime: keystrokesWithIds[0].timestamp,
              endTime: keystrokesWithIds[keystrokesWithIds.length - 1].timestamp
            });
          } else {
            setSequenceStats({ totalKeys: 0, startTime: null, endTime: null });
          }
        }
      } catch (err) {
        console.error('Error fetching sequences:', err);
        setError('Failed to load keystroke sequences');
      } finally {
        //setLoading(false);
      }
    };

    fetchSequences();
  }, [selectedDevice, selectedDate, user]);

  useEffect(() => {
    const fetchScreenshots = async () => {
      if (!selectedDevice || !selectedDate || !user) return;
      
      try {
        setError(null);
        
        // Get screenshots for the selected device and date
        const result = await screenshotAPI.getScreenshots(selectedDevice, selectedDate);
        if (result.success && result.screenshots) {
          setScreenshots(result.screenshots);
        }
      } catch (err) {
        console.error('Error fetching screenshots:', err);
        setError('Failed to load screenshots');
      }
    };

    fetchScreenshots();
  }, [selectedDevice, selectedDate, user]);





  return (
    <div className="p-6 space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-white mb-2">Device Management</h1>
        <p className="text-gray-400">Select a device to view its keystroke sequence</p>
      </div>

      {/* Error Message */}
      {error && (
        <div className="bg-red-500/10 border border-red-500/30 rounded-lg p-4">
          <p className="text-red-400">Error: {error}</p>
        </div>
      )}

      {/* First Row: Devices List + Calendar */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Devices List with Scroll */}
        <div className="space-y-4">
          <h2 className="text-xl font-bold text-white">Your Devices</h2>
          {loading ? (
            <div className="flex items-center justify-center py-8">
              <div className="neon-spinner"></div>
            </div>
          ) : devices.length === 0 ? (
            <div className="neon-card bg-gray-800/50 rounded-xl p-8 text-center">
              <Monitor className="w-12 h-12 text-gray-500 mx-auto mb-4" />
              <p className="text-gray-400">No devices found</p>
              <p className="text-gray-500 text-sm mt-2">Add a device to start tracking</p>
            </div>
          ) : (
            <div className="bg-gray-800/50 rounded-xl p-4 max-h-[600px] overflow-y-auto custom-scrollbar">
              <div className="">
                {devices.map((device) => (
                  <div key={device.id} className="mb-3 last:mb-0">
                    <DeviceCard
                      device={device}
                      isSelected={selectedDevice === device.id}
                      onClick={setSelectedDevice}
                    />
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>

        {/* Calendar Section */}
        <div className="space-y-4 h-full flex flex-col">
          <h2 className="text-xl font-bold text-white">Calendar</h2>
          {selectedDevice ? (
            <>
              {/* Custom Calendar for Date Selection */}
              <CustomCalendar
                selectedDevice={selectedDevice}
                onDateSelect={setSelectedDate}
                initialDate={selectedDate}
              />
              
              {/* Device Info */}
              {selectedDate && (
                <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-4">
                  <p className="text-gray-400 text-center">
                    Showing sequence for: <span className="text-cyan-400 font-medium">
                      {devices.find(d => d.id === selectedDevice)?.computerName || selectedDevice}
                    </span> on {new Date(selectedDate).toLocaleDateString()}
                  </p>
                </div>
              )}
            </>
          ) : (
            <div className="neon-card bg-gray-800/50 rounded-xl p-12 text-center flex-1 flex flex-col items-center justify-center">
              <Monitor className="w-16 h-16 text-gray-500 mb-4" />
              <p className="text-gray-400 text-lg">Select a device to view calendar</p>
            </div>
          )}
        </div>
      </div>

      {/* Second Row: Tab Switcher + Content (Full Width) */}
      <div className="w-full space-y-4">
        {selectedDevice ? (
          <>
            {/* Tab Switcher */}
            <div className="relative border-b border-gray-700/50">
              <div className="flex justify-center space-x-8">
                <button
                  onClick={() => setActiveTab('keystrokes')}
                  className={`relative flex items-center space-x-2 px-4 py-3 font-medium transition-colors ${
                    activeTab === 'keystrokes'
                      ? 'text-cyan-400'
                      : 'text-gray-400 hover:text-gray-300'
                  }`}
                >
                  <Keyboard className="w-5 h-5" />
                  <span>Keystrokes</span>
                  {activeTab === 'keystrokes' && (
                    <div className="absolute bottom-0 left-0 right-0 h-0.5 bg-gradient-to-r from-cyan-400 to-purple-400" />
                  )}
                </button>
                <button
                  onClick={() => setActiveTab('screenshots')}
                  className={`relative flex items-center space-x-2 px-4 py-3 font-medium transition-colors ${
                    activeTab === 'screenshots'
                      ? 'text-cyan-400'
                      : 'text-gray-400 hover:text-gray-300'
                  }`}
                >
                  <Camera className="w-5 h-5" />
                  <span>Screenshots</span>
                  {activeTab === 'screenshots' && (
                    <div className="absolute bottom-0 left-0 right-0 h-0.5 bg-gradient-to-r from-cyan-400 to-purple-400" />
                  )}
                </button>
              </div>
            </div>

            {/* Tab Content */}
            {activeTab === 'keystrokes' ? (
              <KeystrokeSequence 
                keySequence={keySequence} 
                sequenceStats={sequenceStats} 
              />
            ) : (
              <ScreenshotsGallery screenshots={screenshots} />
            )}
          </>
        ) : (
          <div className="neon-card bg-gray-800/50 rounded-xl p-12 text-center">
            <Monitor className="w-16 h-16 text-gray-500 mx-auto mb-4" />
            <p className="text-gray-400 text-lg">Select a device to view data</p>
          </div>
        )}
      </div>
    </div>
  );
};

export default DevicesList;