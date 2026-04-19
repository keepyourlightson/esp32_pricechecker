#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Crypto Tracker Setup</title>
    <style>
        :root {
            --bg-color: #0b0f19;
            --surface-color: rgba(26, 32, 53, 0.6);
            --surface-border: rgba(255, 255, 255, 0.08);
            --primary: #4F46E5;
            --primary-hover: #4338CA;
            --accent: #06B6D4;
            --text-main: #F3F4F6;
            --text-muted: #9CA3AF;
            --danger: #EF4444;
            --danger-hover: #DC2626;
            --success: #10B981;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
        }

        body {
            background-color: var(--bg-color);
            background-image: 
                radial-gradient(at 0% 0%, rgba(79, 70, 229, 0.15) 0px, transparent 50%),
                radial-gradient(at 100% 0%, rgba(6, 182, 212, 0.15) 0px, transparent 50%);
            color: var(--text-main);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: flex-start;
            padding: 2rem 1rem;
        }

        .container {
            width: 100%;
            max-width: 480px;
            background: var(--surface-color);
            backdrop-filter: blur(16px);
            border: 1px solid var(--surface-border);
            border-radius: 24px;
            padding: 2rem;
            box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5);
            animation: slideIn 0.5s ease-out;
        }

        @keyframes slideIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }

        header {
            text-align: center;
            margin-bottom: 2rem;
        }

        h1 {
            font-size: 1.5rem;
            font-weight: 700;
            background: linear-gradient(to right, var(--primary), var(--accent));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 0.5rem;
        }

        .tabs {
            display: flex;
            gap: 1rem;
            margin-bottom: 1.5rem;
            border-bottom: 1px solid var(--surface-border);
            padding-bottom: 1rem;
        }

        .tab {
            flex: 1;
            padding: 0.5rem;
            background: none;
            border: none;
            color: var(--text-muted);
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            border-radius: 8px;
        }

        .tab:hover {
            color: var(--text-main);
            background: rgba(255, 255, 255, 0.05);
        }

        .tab.active {
            color: var(--text-main);
            background: rgba(255, 255, 255, 0.1);
        }

        .view {
            display: none;
            animation: fadeIn 0.3s ease-out;
        }

        .view.active {
            display: block;
        }

        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .form-group {
            margin-bottom: 1.25rem;
        }

        label {
            display: block;
            font-size: 0.875rem;
            font-weight: 500;
            color: var(--text-muted);
            margin-bottom: 0.5rem;
        }

        select, input {
            width: 100%;
            background: rgba(0, 0, 0, 0.2);
            border: 1px solid var(--surface-border);
            color: var(--text-main);
            padding: 0.75rem 1rem;
            border-radius: 12px;
            font-size: 1rem;
            transition: border-color 0.2s, background-color 0.2s;
            outline: none;
        }

        select:focus, input:focus {
            border-color: var(--primary);
            background: rgba(0, 0, 0, 0.3);
        }

        button.btn-primary {
            width: 100%;
            background: linear-gradient(135deg, var(--primary), var(--accent));
            color: white;
            border: none;
            padding: 0.875rem;
            border-radius: 12px;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s, opacity 0.2s;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 0.5rem;
        }

        button.btn-primary:hover {
            opacity: 0.9;
            transform: translateY(-1px);
        }

        button.btn-primary:active {
            transform: translateY(1px);
        }

        button:disabled {
            opacity: 0.5 !important;
            cursor: not-allowed !important;
            transform: none !important;
        }

        .asset-list {
            display: flex;
            flex-direction: column;
            gap: 1rem;
        }

        .asset-card {
            background: rgba(0, 0, 0, 0.2);
            border: 1px solid var(--surface-border);
            border-radius: 16px;
            padding: 1rem;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: transform 0.2s;
        }

        .asset-card:hover {
            transform: scale(1.02);
            background: rgba(0, 0, 0, 0.3);
        }

        .asset-info {
            display: flex;
            flex-direction: column;
            gap: 0.25rem;
        }

        .asset-symbol {
            font-weight: 700;
            font-size: 1.125rem;
        }

        .asset-price {
            color: var(--success);
            font-family: monospace;
            font-size: 0.875rem;
        }
        
        .asset-chain {
            font-size: 0.75rem;
            color: var(--text-muted);
            text-transform: capitalize;
        }

        .btn-icon {
            background: rgba(239, 68, 68, 0.1);
            color: var(--danger);
            border: none;
            width: 36px;
            height: 36px;
            border-radius: 8px;
            cursor: pointer;
            display: flex;
            justify-content: center;
            align-items: center;
            transition: all 0.2s;
        }

        .btn-icon:hover {
            background: var(--danger);
            color: white;
        }

        .limit-msg {
            text-align: center;
            color: var(--danger);
            font-size: 0.875rem;
            margin-top: 1rem;
            display: none;
        }

        .empty-state {
            text-align: center;
            color: var(--text-muted);
            padding: 2rem 0;
            font-size: 0.875rem;
        }

        .status-dot {
            display: inline-block;
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: var(--danger);
            margin-right: 4px;
        }
        .status-dot.connected {
            background-color: var(--success);
            box-shadow: 0 0 8px var(--success);
        }
        .connection-status {
            font-size: 0.75rem;
            color: var(--text-muted);
            margin-top: 0.5rem;
            display: flex;
            align-items: center;
            justify-content: center;
        }
    </style>
</head>
<body>

<div class="container">
    <header>
        <h1>ESP32 Tracker</h1>
        <p style="color: var(--text-muted); font-size: 0.875rem;">Manage up to 5 tracked assets</p>
        <div class="connection-status">
            <span class="status-dot" id="espStatusDot"></span>
            <span id="espStatusText">ESP32 Offline</span>
        </div>
    </header>

    <div class="tabs">
        <button class="tab active" onclick="switchTab('add')">Add Asset</button>
        <button class="tab" onclick="switchTab('list')">Tracked Assets (<span id="assetCount">0</span>/5)</button>
    </div>

    <!-- ADD ASSET VIEW -->
    <div id="view-add" class="view active">
        <div class="form-group">
            <label for="chain">Select Chain</label>
            <select id="chain">
                <option value="ethereum">Ethereum (ETH)</option>
                <option value="solana">Solana (SOL)</option>
                <option value="base">Base</option>
                <option value="bsc">BNB Smart Chain (BSC)</option>
                <option value="arbitrum">Arbitrum</option>
                <option value="polygon">Polygon</option>
            </select>
        </div>

        <div class="form-group">
            <label for="contract">Contract Address</label>
            <input type="text" id="contract" placeholder="e.g. 0x..." autocomplete="off" spellcheck="false">
        </div>

        <button class="btn-primary" id="btnAdd" onclick="addAsset()">
            <svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M12 4v16m8-8H4"></path></svg>
            Add to Tracker
        </button>

        <p class="limit-msg" id="limitMsg">You have reached the maximum limit of 5 assets.</p>
    </div>

    <!-- ASSETS LIST VIEW -->
    <div id="view-list" class="view">
        <div class="asset-list" id="assetList">
            <!-- Populated by JS -->
            <div class="empty-state">No assets tracked yet.</div>
        </div>
    </div>
</div>

<script>
    const MAX_ASSETS = 5;
    let trackedAddresses = [];
    let isConnected = false;

    // Check connection with mock fallback for local dev
    async function loadConfig() {
        try {
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 2000);
            const res = await fetch('/api/assets', { signal: controller.signal });
            clearTimeout(timeoutId);
            
            if (res.ok) {
                const data = await res.json();
                trackedAddresses = data.addresses || [];
                setConnectionStatus(true);
            } else {
                setConnectionStatus(false);
            }
        } catch (e) {
            console.warn("Failed to fetch from ESP32. Assuming local test mode.", e);
            setConnectionStatus(false);
            if(window.location.protocol === 'file:') {
                trackedAddresses = JSON.parse(localStorage.getItem('mockAddresses') || '[]');
            }
        }
        updateUI();
    }

    async function saveConfig() {
        if(window.location.protocol === 'file:') {
            localStorage.setItem('mockAddresses', JSON.stringify(trackedAddresses));
            console.log("Saved local mockup", trackedAddresses);
            return;
        }

        try {
            await fetch('/api/assets', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ addresses: trackedAddresses })
            });
            setConnectionStatus(true);
        } catch(e) {
            setConnectionStatus(false);
            console.error("Save config failed", e);
        }
    }

    function setConnectionStatus(connected) {
        isConnected = connected;
        document.getElementById('espStatusDot').className = connected ? 'status-dot connected' : 'status-dot';
        document.getElementById('espStatusText').innerText = connected ? 'Connected to ESP32' : (window.location.protocol === 'file:' ? 'Local Test Mode' : 'ESP32 Offline');
    }

    function switchTab(tabId) {
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
        
        event.currentTarget.classList.add('active');
        document.getElementById('view-' + tabId).classList.add('active');
        
        if(tabId === 'list') {
            renderList();
        }
    }

    async function renderList() {
        const listEl = document.getElementById('assetList');
        if (trackedAddresses.length === 0) {
            listEl.innerHTML = '<div class="empty-state">No assets tracked yet.</div>';
            return;
        }

        listEl.innerHTML = '<div class="empty-state">Loading data from DexScreener...</div>';
        
        try {
            const joined = trackedAddresses.join(',');
            const res = await fetch(`https://api.dexscreener.com/latest/dex/tokens/${joined}`);
            const data = await res.json();
            
            if (!data.pairs) {
                listEl.innerHTML = '<div class="empty-state">Error fetching data. Check addresses.</div>';
                
                // Allow removing even if offline/error
                listEl.innerHTML = '';
             trackedAddresses.forEach(addr => {
                const card = document.createElement('div');
                card.className = 'asset-card';
                card.innerHTML = `
                        <div class="asset-info">
                            <span class="asset-symbol">Offline/Error Data</span>
                            <span class="asset-chain">${addr.substring(0,6)}...${addr.substring(addr.length-4)}</span>
                        </div>
                        <button class="btn-icon" onclick="removeAsset('${addr}')" title="Stop Tracking">
                            <svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path></svg>
                        </button>
                    `;
                 listEl.appendChild(card);
             });
                
                return;
            }

            const uniquePairs = [];
            const seen = new Set();
            data.pairs.forEach(p => {
                const addr = p.baseToken.address.toLowerCase();
                if (!seen.has(addr) && trackedAddresses.some(t => t.toLowerCase() === addr)) {
                    seen.add(addr);
                    uniquePairs.push(p);
                }
            });

            listEl.innerHTML = '';
            
            trackedAddresses.forEach(addr => {
                const pair = uniquePairs.find(p => p.baseToken.address.toLowerCase() === addr.toLowerCase());
                const card = document.createElement('div');
                card.className = 'asset-card';
                
                if (pair) {
                    card.innerHTML = `
                        <div class="asset-info">
                            <span class="asset-symbol">${pair.baseToken.symbol}</span>
                            <span class="asset-chain">${pair.chainId}</span>
                            <span class="asset-price">$${Number(pair.priceUsd).toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 6})}</span>
                        </div>
                        <button class="btn-icon" onclick="removeAsset('${addr}')" title="Stop Tracking">
                            <svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path></svg>
                        </button>
                    `;
                } else {
                    card.innerHTML = `
                        <div class="asset-info">
                            <span class="asset-symbol">Unknown Token</span>
                            <span class="asset-chain">${addr.substring(0,6)}...${addr.substring(addr.length-4)}</span>
                        </div>
                        <button class="btn-icon" onclick="removeAsset('${addr}')" title="Stop Tracking">
                            <svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path></svg>
                        </button>
                    `;
                }
                listEl.appendChild(card);
            });

        } catch (e) {
            console.error(e);
            listEl.innerHTML = '<div class="empty-state">Failed to reach DexScreener.</div>';
            
            // Still allow deleting offline tokens
            listEl.innerHTML = '';
             trackedAddresses.forEach(addr => {
                const card = document.createElement('div');
                card.className = 'asset-card';
                card.innerHTML = `
                        <div class="asset-info">
                            <span class="asset-symbol">Offline Data</span>
                            <span class="asset-chain">${addr.substring(0,6)}...${addr.substring(addr.length-4)}</span>
                        </div>
                        <button class="btn-icon" onclick="removeAsset('${addr}')" title="Stop Tracking">
                            <svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"></path></svg>
                        </button>
                    `;
                 listEl.appendChild(card);
             });
        }
    }

    async function addAsset() {
        const addrInput = document.getElementById('contract');
        const addr = addrInput.value.trim();
        const btn = document.getElementById('btnAdd');

        if (!addr) {
            alert('Please enter a valid contract address');
            return;
        }

        if (trackedAddresses.length >= MAX_ASSETS) {
            return;
        }

        if (trackedAddresses.some(a => a.toLowerCase() === addr.toLowerCase())) {
            alert('This asset is already tracked.');
            return;
        }

        btn.disabled = true;
        btn.innerHTML = 'Verifying...';

        try {
            const res = await fetch(`https://api.dexscreener.com/latest/dex/tokens/${addr}`);
            const data = await res.json();
            
            if (!data.pairs || data.pairs.length === 0) {
                alert('Token not found on DexScreener. Please verify the contract address.');
                btn.disabled = false;
                btn.innerHTML = '<svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M12 4v16m8-8H4"></path></svg> Add to Tracker';
                return;
            }

            trackedAddresses.push(addr);
            await saveConfig();
            
            addrInput.value = '';
            updateUI();
            
            alert('Added successfully!');
            switchTab('list');
            document.querySelectorAll('.tab')[1].classList.add('active');
            document.querySelectorAll('.tab')[0].classList.remove('active');

        } catch (e) {
            alert('Error connecting to DexScreener.');
            console.error(e);
        }

        btn.disabled = false;
        btn.innerHTML = '<svg width="20" height="20" fill="none" stroke="currentColor" stroke-width="2" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" d="M12 4v16m8-8H4"></path></svg> Add to Tracker';
    }

    async function removeAsset(addr) {
        trackedAddresses = trackedAddresses.filter(a => a.toLowerCase() !== addr.toLowerCase());
        await saveConfig();
        updateUI();
        renderList();
    }

    function updateUI() {
        const countSpan = document.getElementById('assetCount');
        countSpan.innerText = trackedAddresses.length;

        const btnAdd = document.getElementById('btnAdd');
        const limitMsg = document.getElementById('limitMsg');

        if (trackedAddresses.length >= MAX_ASSETS) {
            btnAdd.style.display = 'none';
            limitMsg.style.display = 'block';
        } else {
            btnAdd.style.display = 'flex';
            limitMsg.style.display = 'none';
        }
    }

    const link = document.createElement('link');
    link.href = 'https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap';
    link.rel = 'stylesheet';
    document.head.appendChild(link);

    loadConfig();
</script>
</body>
</html>
)=====";

#endif
