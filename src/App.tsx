import React, { useState, useEffect } from 'react';
import { motion } from 'motion/react';
import { Volume2, VolumeX, Eye, EyeOff, LayoutPanelLeft, Layers } from 'lucide-react';

const BandControl = ({ index }: { index: number }) => {
  const [width, setWidth] = useState(100);
  const [widener, setWidener] = useState(0);
  const [isSolo, setIsSolo] = useState(false);
  const [isMute, setIsMute] = useState(false);
  const [mode, setMode] = useState('Stereo');

  const rotation = (width / 200) * 270 - 135;

  return (
    <div className="flex-1 flex flex-col items-center bg-[#111] border border-white/5 rounded-sm p-3 gap-4 shadow-inner">
      <div className="w-full">
        <select 
          value={mode}
          onChange={(e) => setMode(e.target.value)}
          className="w-full bg-[#0a0a0a] text-[10px] font-mono border border-white/10 rounded px-1 py-0.5 text-cyan-400 outline-none"
        >
          <option>Stereo</option>
          <option>Mid</option>
          <option>Side</option>
        </select>
      </div>

      <div className="relative flex flex-col items-center group">
        <span className="text-[10px] font-mono text-gray-500 mb-2 uppercase tracking-widest group-hover:text-cyan-400 transition-colors">Stereo Enhancer</span>
        <div className="relative w-16 h-16 rounded-full bg-gradient-to-b from-[#222] to-[#0a0a0a] border-2 border-[#333] flex items-center justify-center shadow-lg cursor-pointer overflow-hidden">
          <motion.div 
            style={{ rotate: rotation }}
            className="absolute inset-0 flex items-start justify-center pt-1"
          >
            <div className="w-1 h-3 bg-cyan-500 rounded-full shadow-[0_0_8px_cyan]" />
          </motion.div>
          <span className="text-[10px] font-mono text-gray-400 z-10">{Math.round(width * 1.5)}%</span>
        </div>
      </div>

      <div className="w-full h-24 bg-[#0a0a0a] border border-white/5 rounded flex flex-col items-center py-2 relative overflow-hidden">
        <div className="absolute inset-0 bg-[linear-gradient(transparent_95%,rgba(0,255,255,0.05)_95%)] bg-[length:100%_4px]" />
        <span className="text-[9px] font-mono text-gray-600 mb-1 uppercase text-center">Widener<br/>{Math.round(widener * 1.5)}%</span>
        <div className="flex-1 w-2 bg-[#1a1a1a] rounded-full relative overflow-hidden border border-black">
          <div 
            className="absolute bottom-0 w-full bg-cyan-600 shadow-[0_0_10px_rgba(0,255,255,0.3)]" 
            style={{ height: `${widener}%` }} 
          />
        </div>
      </div>

      <div className="flex gap-2 w-full">
        <button 
          onClick={() => setIsSolo(!isSolo)}
          className={`flex-1 py-1 rounded text-[10px] font-bold border transition-all ${isSolo ? 'bg-yellow-500/20 text-yellow-500 border-yellow-500 shadow-[0_0_10px_rgba(234,179,8,0.2)]' : 'bg-[#1a1a1a] text-gray-500 border-white/5'}`}
        >
          S
        </button>
        <button 
          onClick={() => setIsMute(!isMute)}
          className={`flex-1 py-1 rounded text-[10px] font-bold border transition-all ${isMute ? 'bg-red-500/20 text-red-500 border-red-500 shadow-[0_0_10px_rgba(239,68,68,0.2)]' : 'bg-[#1a1a1a] text-gray-500 border-white/5'}`}
        >
          M
        </button>
      </div>
    </div>
  );
};

const Vectorscope = () => {
  const [points, setPoints] = useState<Array<{x: number, y: number}>>([]);

  useEffect(() => {
    const interval = setInterval(() => {
      const newPoints = Array.from({ length: 40 }, () => ({
        x: (Math.random() - 0.5) * 120,
        y: (Math.random() - 0.5) * 120
      }));
      setPoints(newPoints);
    }, 50);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="w-full h-[200px] bg-[#080808] border border-cyan-900/30 rounded-sm relative flex items-center justify-center overflow-hidden">
      {/* Grid */}
      <div className="absolute inset-0 grid grid-cols-6 grid-rows-6 pointer-events-none opacity-10">
        {Array.from({ length: 36 }).map((_, i) => (
          <div key={i} className="border-[0.5px] border-cyan-400" />
        ))}
      </div>
      
      {/* Scope Center Cross */}
      <div className="absolute w-full h-[0.5px] bg-cyan-900/30" />
      <div className="absolute h-full w-[0.5px] bg-cyan-900/30" />
      
      {/* Side indicators */}
      <div className="absolute left-2 text-[8px] font-mono text-cyan-900 uppercase">Left</div>
      <div className="absolute right-2 text-[8px] font-mono text-cyan-900 uppercase">Right</div>
      <div className="absolute top-2 text-[8px] font-mono text-cyan-900 uppercase">Side</div>
      <div className="absolute bottom-2 text-[8px] font-mono text-cyan-900 uppercase">Mid</div>

      {/* Points */}
      <svg className="w-full h-full absolute inset-0 z-10 overflow-visible">
        {points.map((p, i) => (
          <motion.circle
            key={i}
            cx={`${50 + p.x}%`}
            cy={`${50 + p.y}%`}
            r="1.5"
            fill="cyan"
            initial={false}
            animate={{ opacity: [0.8, 0], scale: [1, 1.5] }}
            transition={{ duration: 0.2 }}
            className="blur-[0.5px]"
          />
        ))}
        {/* Draw main trace line */}
        <polyline
          points={points.map(p => `${50 + p.x}%,${50 + p.y}%`).join(' ')}
          fill="none"
          stroke="rgba(0, 255, 255, 0.2)"
          strokeWidth="0.5"
        />
      </svg>
      
      {/* Glow Overlay */}
      <div className="absolute inset-0 bg-gradient-to-t from-cyan-950/10 via-transparent to-transparent pointer-events-none" />
    </div>
  );
};

export default function App() {
  return (
    <div className="min-h-screen flex items-center justify-center p-8 bg-[#050505]">
      <div className="w-[800px] h-[580px] bg-[#0d0d0d] rounded-lg border border-[#222] shadow-2xl flex flex-col overflow-hidden relative font-sans">
        {/* Top Header */}
        <div className="h-12 bg-[#151515] border-b border-white/5 flex items-center px-6 justify-between shrink-0">
          <div className="flex items-center gap-3">
            <div className="w-3 h-3 bg-cyan-500 rounded-sm shadow-[0_0_8px_cyan]" />
            <h1 className="font-orbitron font-bold text-lg tracking-[0.2em] text-cyan-400 uppercase">Nexus Imager</h1>
          </div>
          <div className="flex items-center gap-4 text-[10px] font-mono text-gray-500">
            <div className="flex items-center gap-1.5 hover:text-cyan-400 cursor-pointer transition-colors group">
              <Eye size={12} className="group-hover:scale-110 transition-transform" />
              <span>Vectorscope</span>
            </div>
            <div className="flex items-center gap-1.5 hover:text-cyan-400 cursor-pointer transition-colors group">
              <Layers size={12} className="group-hover:scale-110 transition-transform" />
              <span>Presets</span>
            </div>
            <div className="px-2 py-0.5 border border-white/10 rounded-full text-[8px] bg-black/40">V1.0.0 Pro</div>
          </div>
        </div>

        {/* Main Content */}
        <main className="flex-1 p-6 flex flex-col gap-6">
          <Vectorscope />
          
          <div className="flex gap-4 p-2 bg-black/20 rounded border border-white/5 mb-4 items-center justify-center">
            <div className="flex flex-col items-center">
              <span className="text-[8px] font-mono text-gray-500 mb-1">LOW-MID</span>
              <input type="range" className="w-24 accent-cyan-500 bg-[#222] h-1" />
              <span className="text-[9px] font-mono text-cyan-400">200 Hz</span>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-[8px] font-mono text-gray-500 mb-1">MID-HIGH</span>
              <input type="range" className="w-24 accent-cyan-500 bg-[#222] h-1" />
              <span className="text-[9px] font-mono text-cyan-400">1000 Hz</span>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-[8px] font-mono text-gray-500 mb-1">HIGH</span>
              <input type="range" className="w-24 accent-cyan-500 bg-[#222] h-1" />
              <span className="text-[9px] font-mono text-cyan-400">5000 Hz</span>
            </div>
          </div>
          
          <div className="flex-1 flex gap-4">
            <BandControl index={0} />
            <BandControl index={1} />
            <BandControl index={2} />
            <BandControl index={3} />
          </div>
        </main>

        {/* Footer */}
        <footer className="h-8 bg-[#0a0a0a] border-t border-white/5 px-6 flex items-center justify-between text-[8px] font-mono text-gray-600 shrink-0">
          <div className="flex gap-4">
            <span>PLATINUM DSP ENGINE ACTIVE</span>
            <span>LINEAR PHASE CROSSOVER: 4-BAND</span>
          </div>
          <div className="text-cyan-900 drop-shadow-[0_0_2px_cyan]">NEXUSAUDIO RESEARCH LABORATORIES</div>
        </footer>

        {/* Grain Overlay */}
        <div className="absolute inset-0 pointer-events-none opacity-[0.03] mix-blend-overlay bg-[url('https://grainy-gradients.vercel.app/noise.svg')]" />
      </div>
    </div>
  );
}
