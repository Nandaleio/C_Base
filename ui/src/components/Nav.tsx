import { useLocation } from "preact-iso";
import { useEffect } from "preact/hooks";
import tippy from "tippy.js";
import 'tippy.js/dist/tippy.css';

export function Nav() {
    
	const loc = useLocation();

    useEffect(() => {
        tippy('[data-tippy-content]', {
            placement: "right"
        });
    })
    
    const style = {
        "display": "flex",
        "flex-direction": "column",
        "gap": "1rem",
        "padding": "1rem",
        "background-color": "var(--dark-surface-color)",
   };
    return(
        <div style={style}>
			<button data-tippy-content="Tables" onClick={() => loc.route("tables")}>📊</button>
			<button data-tippy-content="Editor" onClick={() => loc.route("editor")}>✏️</button>
			<button data-tippy-content="Logs" disabled>📄</button>
			<span style="flex-grow: 2;"></span>
			<button data-tippy-content="Settings" disabled>⚙️</button>
		</div>
    )
}