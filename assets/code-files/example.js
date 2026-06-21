// 1. Declare variables
const greeting = "Hello, World!";
let clickCount = 0;

// 2. Define a function to update the webpage
function showMessage() {
    clickCount++;
    
    // Find an HTML element with the ID "output"
    const outputElement = document.getElementById("output");
    
    // Change its text content dynamically
    outputElement.textContent = `${greeting} You clicked the button ${clickCount} times.`;
    
    // Print a message to the browser's developer console
    console.log(`Button clicked! Current count: ${clickCount}`);
}

// 3. Wait for the webpage to load, then attach an event to a button
document.addEventListener("DOMContentLoaded", () => {
    const actionButton = document.getElementById("actionButton");
    
    if (actionButton) {
        actionButton.addEventListener("click", showMessage);
    }
});
