import { createRouter, createWebHistory } from 'vue-router'
import LoginView from '@/views/LoginView.vue'
import TablesView from '@/views/home/TablesView.vue'
import HomeView from '@/views/HomeView.vue'
import EditorView from '@/views/home/EditorView.vue'
import LogsView from '@/views/home/LogsView.vue'
import SettingsView from '@/views/home/SettingsView.vue'
import FunctionsView from '@/views/home/FunctionsView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.VITE_BASE_URL),
  routes: [
    { path: '/login', name: 'login', component: LoginView},
    { path: '/', component: HomeView, children: [
      { path: '/tables/:table*', component: TablesView },
      { path: '/editor', component: EditorView },
      { path: '/functions', component: FunctionsView },
      { path: '/logs', component: LogsView },
      { path: '/settings', component: SettingsView },
      { path: '/:pathMatch(.*)*', redirect: '/tables' }
    ]},
  ],
})

router.beforeEach((to, from, next) => {
  const isAuthenticated = localStorage.getItem(import.meta.env.VITE_LOCALSTORAGE_TOKEN_KEY);
  if (to.name !== 'login' && !isAuthenticated) next({ name: 'login' })
  else next();
})

export default router
