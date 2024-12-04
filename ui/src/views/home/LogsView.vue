<script setup lang="ts">
    import { cbFetch } from '@/services/api-service';
    import { onMounted, ref } from 'vue';

    const loading = ref(false);
    const data = ref<{created: Date, level: string, description: string}[]>([]);
    
    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/admin/logs')
        data.value = res.data;
        loading.value = false;
    }

    onMounted(async () => {
        await query();
    })

</script>

<template>
        <div class="logs-container">
            <div class="actions">
                <button @click="query" :aria-busy="loading">Refresh</button>
            </div>
            <table>
                <thead>
                    <tr>
                    <th>Created</th>
                    <th>Level</th>
                    <th>Description</th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="log of data">
                        <td>{{ log.created }}</td>
                        <td>{{ log.level }}</td>
                        <td>{{ log.description }}</td>
                    </tr>
                </tbody>
            </table>
        </div>
</template>

<style scoped>
    .logs-container {
        padding: 1rem;
    }
</style>