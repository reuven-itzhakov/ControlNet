import React, { useState, useEffect } from 'react';
import { ChevronLeft, ChevronRight } from 'lucide-react';
import { sequenceAPI } from '../../services/api';

// Helper functions
function getDaysInMonth(year, month) {
  return new Date(year, month + 1, 0).getDate();
}

function getFirstDayOfWeek(year, month) {
  return new Date(year, month, 1).getDay();
}

const CustomCalendar = ({ selectedDevice, onDateSelect, initialDate }) => {
  const today = new Date();
  const [currentMonth, setCurrentMonth] = useState(initialDate ? new Date(initialDate).getMonth() : today.getMonth());
  const [currentYear, setCurrentYear] = useState(initialDate ? new Date(initialDate).getFullYear() : today.getFullYear());
  const [selectedDay, setSelectedDay] = useState(initialDate ? new Date(initialDate).getDate() : today.getDate());
  const [datesWithKeystrokes, setDatesWithKeystrokes] = useState(new Set());
  const [loading, setLoading] = useState(false);

  const daysInMonth = getDaysInMonth(currentYear, currentMonth);
  const firstDayOfWeek = getFirstDayOfWeek(currentYear, currentMonth);
  const daysOfWeek = ['Su', 'Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa'];

  // Fetch dates with keystroke data for the selected device
  useEffect(() => {
    async function fetchDatesWithKeystrokes() {
      if (!selectedDevice) {
        setDatesWithKeystrokes(new Set());
        return;
      }

      setLoading(true);
      try {
        const result = await sequenceAPI.getDeviceDates(selectedDevice);
        // Convert YYYY-MM-DD to Set of date strings for quick lookup
        if (result.success && result.dates) {
          const dateSet = new Set(result.dates);
          setDatesWithKeystrokes(dateSet);
        }
      } catch (error) {
        console.error('Error fetching keystroke dates:', error);
        setDatesWithKeystrokes(new Set());
      } finally {
        setLoading(false);
      }
    }

    fetchDatesWithKeystrokes();
  }, [selectedDevice]);

  const prevMonth = () => {
    if (currentMonth === 0) {
      setCurrentMonth(11);
      setCurrentYear(currentYear - 1);
    } else {
      setCurrentMonth(currentMonth - 1);
    }
    setSelectedDay(null);
  };

  const nextMonth = () => {
    if (currentMonth === 11) {
      setCurrentMonth(0);
      setCurrentYear(currentYear + 1);
    } else {
      setCurrentMonth(currentMonth + 1);
    }
    setSelectedDay(null);
  };

  // Check if a date has keystroke data
  const hasKeystrokes = (day) => {
    if (!day) return false;
    const dateStr = `${currentYear}-${String(currentMonth + 1).padStart(2, '0')}-${String(day).padStart(2, '0')}`;
    return datesWithKeystrokes.has(dateStr);
  };

  // Generate calendar grid
  const calendarDays = [];
  for (let i = 0; i < firstDayOfWeek; i++) {
    calendarDays.push(null); // Empty cells for previous month
  }
  for (let day = 1; day <= daysInMonth; day++) {
    calendarDays.push(day);
  }

  const handleDayClick = (day) => {
    if (!day || !hasKeystrokes(day)) return;
    
    setSelectedDay(day);
    const dateStr = `${currentYear}-${String(currentMonth + 1).padStart(2, '0')}-${String(day).padStart(2, '0')}`;
    onDateSelect && onDateSelect(dateStr);
  };

  const monthName = new Date(currentYear, currentMonth).toLocaleString('en', { month: 'long', year: 'numeric' });

  return (
    <div className="neon-card bg-gray-800/50 backdrop-blur-sm border border-cyan-500/30 rounded-xl p-6">
      {/* Month navigation */}
      <div className="flex justify-between items-center mb-4">
        <button
          onClick={prevMonth}
          className="p-2 bg-gray-700/50 hover:bg-gray-600/50 rounded-lg transition-colors duration-300"
          aria-label="Previous month"
        >
          <ChevronLeft className="w-5 h-5 text-gray-300" />
        </button>
        <h3 className="text-lg font-semibold text-white">{monthName}</h3>
        <button
          onClick={nextMonth}
          className="p-2 bg-gray-700/50 hover:bg-gray-600/50 rounded-lg transition-colors duration-300"
          aria-label="Next month"
        >
          <ChevronRight className="w-5 h-5 text-gray-300" />
        </button>
      </div>

      {/* Day names header */}
      <div className="grid grid-cols-7 gap-2 mb-2">
        {daysOfWeek.map((day, i) => (
          <div key={i} className="text-center text-sm font-semibold text-gray-400">
            {day}
          </div>
        ))}
      </div>

      {/* Days grid */}
      <div className="grid grid-cols-7 gap-2">
        {loading ? (
          <div className="col-span-7 text-center py-8">
            <div className="inline-block w-8 h-8 border-4 border-cyan-400 border-t-transparent rounded-full animate-spin"></div>
            <p className="text-gray-400 mt-2">Loading dates...</p>
          </div>
        ) : (
          calendarDays.map((day, idx) => {
            const isEmpty = day === null;
            const hasData = day !== null && hasKeystrokes(day);
            const isSelected = day === selectedDay && currentMonth === (initialDate ? new Date(initialDate).getMonth() : today.getMonth());
            const isToday = day === today.getDate() && 
                           currentMonth === today.getMonth() && 
                           currentYear === today.getFullYear();

            return (
              <div
                key={idx}
                onClick={() => handleDayClick(day)}
                className={`
                  h-12 flex items-center justify-center rounded-lg text-sm font-medium
                  transition-all duration-300 ease-in-out
                  ${isEmpty ? 'invisible' : ''}
                  ${hasData ? 'cursor-pointer' : 'cursor-default'}
                  ${isSelected 
                    ? 'bg-cyan-500 text-gray-900 ring-2 ring-cyan-400 scale-110 shadow-lg shadow-cyan-500/50' 
                    : hasData 
                      ? 'bg-gradient-to-br from-cyan-500/30 to-purple-500/30 text-white hover:from-cyan-500/50 hover:to-purple-500/50 hover:scale-105 border border-cyan-500/30'
                      : 'bg-gray-700/30 text-gray-500'
                  }
                  ${isToday && !isSelected ? 'ring-2 ring-cyan-500/50' : ''}
                `}
              >
                {day || ''}
                {hasData && !isSelected && (
                  <span className="absolute mt-8 w-1.5 h-1.5 bg-cyan-400 rounded-full"></span>
                )}
              </div>
            );
          })
        )}
      </div>

      {/* Selected day info */}
      {selectedDay && (
        <div className="mt-4 pt-4 border-t border-gray-600/30">
          <p className="text-center text-sm text-cyan-400 font-medium">
            Selected: {selectedDay}/{currentMonth + 1}/{currentYear}
          </p>
        </div>
      )}

      {/* Legend */}
      {!loading && (
        <div className="mt-4 pt-4 border-t border-gray-600/30 flex items-center justify-center space-x-4 text-xs">
          <div className="flex items-center space-x-1.5">
            <div className="w-3 h-3 rounded bg-gradient-to-br from-cyan-500/30 to-purple-500/30 border border-cyan-500/30"></div>
            <span className="text-gray-400">Has Data</span>
          </div>
          <div className="flex items-center space-x-1.5">
            <div className="w-3 h-3 rounded bg-gray-700/30"></div>
            <span className="text-gray-400">No Data</span>
          </div>
        </div>
      )}
    </div>
  );
};

export default CustomCalendar;
