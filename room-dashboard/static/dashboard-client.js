let lastLight = false;
let lastRollerBlind = 0;

// Function to send the light change
const toggleLightButton = document.getElementById('toggleLightButton');
toggleLightButton.addEventListener('click', () => {
    if (toggleLightButton.getAttribute('data-state') === 'on') {
        fetch(new Request('/set-light', { method: 'POST', body: 'off'}));
    } else {
        fetch(new Request('/set-light', { method: 'POST', body: 'on'}));
    }
});

// Function to send the blind value change
var slider = document.getElementById('blindSlider');
var blindValueLabel = document.getElementById('blindValue');
function updateBlind() {
    fetch(new Request('/set-roller-blind', { method: 'POST', body: slider.value.toString()}));
}

// Create the Light "On/Off" chart
var ctxLight = document.getElementById('lightChart').getContext('2d');
var lightChart = new Chart(ctxLight, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Light "On/Off"',
            data: [],
            stepped: 'middle',
            pointRadius: 0,
            borderJoinStyle: 'round',
            borderWidth: 2,
            borderColor: 'rgba(75, 192, 192, 1)',
            fill: true,
            steppedLine: true // Set the line to stepped
        }]
    },
    options: {
        clip: false,
        scales: {
            y: {
                beginAtZero: true,
                max: 1,
                stepSize: 1,
                title: {
                    display: true,
                    text: 'State'
                },
                ticks: {
                    stepSize: 1,
                    callback: function(value, index, values) {
                        switch  (value) {
                            case 0: return "Off";
                            case 1: return "On";
                        }
                    }
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Hours'
                },
                type: 'time',
                time: {
                    unit: 'second'
                },
                ticks: {
                    autoSkip: false, // Disable auto-skipping of labels
                    maxRotation: 45, // Rotate labels by 45 degrees
                    minRotation: 45 // Rotate labels by 45 degrees
                }
            }
        },
        layout: {
            padding: {
                top: 10,
                bottom: 10
            }
        }
    }
});

// Create the chart for values from 0 to 100
var ctxRollerBlind = document.getElementById('rollerBlindChart').getContext('2d');
var rollerBlindChart = new Chart(ctxRollerBlind, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Values from 0 to 100',
            data: [],
            stepped: 'middle',
            pointRadius: 0,
            borderJoinStyle: 'round',
            borderWidth: 2,
            borderColor: 'rgba(255, 99, 132, 1)',
            fill: 'end'
        }]
    },
    options: {
        clip: false,
        scales: {
            y: {
                beginAtZero: true,
                max: 100,
                title: {
                    display: true,
                    text: 'Values'
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Hours'
                },
                type: 'time',
                time: {
                    unit: 'second'
                },
                ticks: {
                    autoSkip: false, // Disable auto-skipping of labels
                    maxRotation: 45, // Rotate labels by 45 degrees
                    minRotation: 45 // Rotate labels by 45 degrees
                }
            }
        },
        layout: {
            padding: {
                top: 10,
                bottom: 10
            }
        }
    }
});

async function updateLightChart() {
    try {
        let res = await fetch(new Request('http://localhost:5000/light-chart-values'), { method: 'GET' });
        
        let res_data = await res.json();
        let value
        let timestamps = res_data.map(v => v.ts);
        let values = res_data.map(v => v.state);
        lastLight = values[values.length - 1];
        lightChart.data.datasets[0].data = values;
        lightChart.data.labels = timestamps;
        lightChart.update();
        
    } catch (error) {
        console.log('Fetching chart data has failed.');
    }
}

async function updateRollerBlindChart() {
    try {
        let res = await fetch(new Request('http://localhost:5000/roller-blind-chart-values'), { method: 'GET' });
        
        let res_data = await res.json();
        let timestamps = res_data.map(v => v.ts);
        let values = res_data.map(v => v.state);
        lastRollerBlind = values[values.length - 1];
        rollerBlindChart.data.datasets[0].data = values;
        rollerBlindChart.data.labels = timestamps;
        rollerBlindChart.update();
        
    } catch (error) {
        console.log('Fetching chart data has failed.');
    }
}

function updateButtons() {
    if (lastLight === true) {
        toggleLightButton.setAttribute('data-state', 'on');
        toggleLightButton.textContent = 'On';
    } else {
        toggleLightButton.setAttribute('data-state', 'off');
        toggleLightButton.textContent = 'Off';
    }
    if (slider.value !== lastRollerBlind) {
        slider.value = lastRollerBlind;
        blindValueLabel.textContent = lastRollerBlind + '%';
    }
}

function updateDashboard() {
    updateLightChart();
    updateRollerBlindChart();
    updateButtons();

    setTimeout(updateDashboard, 1000);
}
updateDashboard();