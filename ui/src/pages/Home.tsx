import { Router } from 'preact-iso';
import { ProtectedRoute } from '../components/ProtectedRoute';
import { Editor } from './Editor';
import { Tables } from './Tables';

export function Home() {
	return (
		<>
			<div class="nav">
			<a href="/tables">Tables</a>
				<button>Tables</button>
				<a href="/editor">Editor</a>
				<button>Editor</button>
				<button disabled>Logs</button>
				<span class="separator"></span>
				<button disabled>Settings</button>
			</div>
			<div class="content">
				<Router>
					<ProtectedRoute path="/tables" component={Tables} />
					<ProtectedRoute path="/editor" component={Editor} />
				</Router>
			</div>
		</>
	);
}
