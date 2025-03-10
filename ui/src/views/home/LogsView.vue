<script setup lang="ts">

import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
    } from '@/components/ui/table'
    import { Button } from '@/components/ui/button'
    import { cbFetch } from '@/services/api-service';
    import { onMounted, ref } from 'vue';

    import { AreaChart } from '@/components/ui/chart-area'
    import { CurveType } from '@unovis/ts'
import type { Log } from '@/utils/types';

    const loading = ref(false);
    const data = ref<Log[]>([]);
    
    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/admin/logs')
        data.value = res.data;
        loading.value = false;
    }

    function mapDataToDateTotals(): {date:string, info: number, error: number}[] {
        const dateCountMap: {[key:string]: {info: number, error: number}} = {};

        data.value.sort((a,b) => a.created - b.created).forEach(item => {
            const dateKey = item.created.toString().split(' ')[0];
            if(!dateCountMap[dateKey]) {
                dateCountMap[dateKey] = {
                    info: 0,
                    error: 0
                };
            }
            if(item.level === 'INFO') {
                dateCountMap[dateKey].info = dateCountMap[dateKey].info + 1;
            } else {
                dateCountMap[dateKey].error = dateCountMap[dateKey].error + 1;
            }
        });

        const result = Object.entries(dateCountMap).map(([date, total]) => ({
            date: new Date(date).toLocaleString('en-GB', { timeZone: 'UTC' }),
            info: total.info,
            error: total.error
        }));

        return result;
    }

    onMounted(async () => {
        await query();
    })

</script>

<template>
        <div class="logs-container p-4">
            <div class="actions">
                <Button variant="outline" @click="query" :aria-busy="loading">Refresh</Button>
            </div>

            <AreaChart
                class="h-[150px] w-full mb-8"
                index="date"
                :data="mapDataToDateTotals()"
                :categories="['error','info']"
                :curve-type="CurveType.Natural"
                :colors="['red', 'green']"
            />

            <Table>
                <TableHeader>
                <TableRow>
                    <TableHead>Created</TableHead>
                    <TableHead>Level</TableHead>
                    <TableHead>Description</TableHead>
                </TableRow>
                </TableHeader>
                <TableBody>
                    <TableRow v-for="d of data">
                        <TableCell>{{ new Date(d.created).toLocaleString('en-GB', { timeZone: 'UTC' }) }}</TableCell>
                        <TableCell>{{d.level}}</TableCell>
                        <TableCell>{{d.description}}</TableCell>
                    </TableRow>
                </TableBody>
            </Table>
        </div>
</template>