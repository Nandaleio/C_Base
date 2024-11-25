import { createRouter, createWebHistory } from 'vue-router'
import LoginView from '@/views/LoginView.vue'
import TableView from '@/views/home/TableView.vue'
import { env } from '@/utils/env'
import HomeView from '@/views/HomeView.vue'
import EditorView from '@/views/home/EditorView.vue'
import LogsView from '@/views/home/LogsView.vue'
import SettingsView from '@/views/home/SettingsView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    { path: '/login', name: 'login', component: LoginView},
    { path: '/', component: HomeView, children: [
      { path: '/tables', component: TableView },
      { path: '/editor', component: EditorView },
      { path: '/logs', component: LogsView },
      { path: '/settings', component: SettingsView },
      { path: '/:pathMatch(.*)*', redirect: '/tables' }
    ]},
  ],
})

router.beforeEach((to, from, next) => {
  const isAuthenticated = localStorage.getItem(env.localStorageTokenKey);
  if (to.name !== 'login' && !isAuthenticated) next({ name: 'login' })
  else next()
})

export default router
