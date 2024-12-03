<script setup lang="ts">
    import { cbFetch } from '@/services/api-service';
    import { ref } from 'vue';

    const sqlQuery = ref('');

    const loading = ref(false);
    const error = ref();
    const cols = ref([]);
    const data = ref([]);

    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/query', "POST", {query: sqlQuery.value});
        cols.value = res.columns;
        data.value = res.data;
        error.value = res.error;
        loading.value = false;
    }
</script>

<template>
    <div class="main-container">
        <textarea v-model="sqlQuery" name="sqlQuery"></textarea>
        <div class="actions">
            <button class="primary" @click="query">run</button>
        </div>
        <div class="content" :aria-busy="loading">
            <table v-if="cols && cols.length">
                <thead>
                    <tr>
                    <th v-for="col of cols">
                        {{ col }}
                    </th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="d of data">
                        <td v-for="col of cols">
                            {{ d[col] }}
                        </td>
                    </tr>
                </tbody>
            </table>
            <pre v-else-if="error" class="error">{{ error }}</pre>
        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    flex-direction: column;
    padding: 1rem;
    gap: 1rem;
    
    .actions{
        display: flex;
    }

    .content {

        display: flex;
        align-items: flex-start;
        justify-content: center;
        height: 100%;

        table {
            width: 100%;
        }

        .error{
            color: red;
        }
    }

}

</style>