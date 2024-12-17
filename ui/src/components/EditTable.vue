<script setup lang="ts">
import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger
} from '@/components/ui/dialog'

import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import { AlertDialogTrigger, AlertDialog } from '@/components/ui/alert-dialog'
import AlertConfirm from '@/components/AlertConfirm.vue';
import { Button } from '@/components/ui/button'
import { Trash2, Settings } from 'lucide-vue-next'
import { ref } from 'vue';
import { cbFetch } from '@/services/api-service';

import ColumnDefinition from '@/components/ColumnDefinition.vue';
import type { Cols } from '@/utils/types';

const props = defineProps<{
    tableName: string,
    cols?: Cols[]
}>()

const open = ref();

async function deleteTable() {
    await cbFetch(`/api/admin/table/${props.tableName}`, 'DELETE');

}

const emit = defineEmits<{
    updated: []
    deleted: []
}>()
</script>

<template>

    <Dialog :open="open" @update:open="(o) => open = o">
        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Button variant="outline">
                            <Settings />
                        </Button>
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Edit table</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>

        <DialogContent>
            <DialogHeader>
                <DialogTitle>Edit the table {{ tableName }}</DialogTitle>
                <DialogDescription>

                </DialogDescription>
            </DialogHeader>

            <div class="flex items-center" v-for="col of cols">
                <ColumnDefinition :col="col" @delete="cols?.splice(cols?.indexOf(col), 1)" />
            </div>

            <Button variant="outline" @click="cols?.push({name:'', type:'', options: {}})">Add Column</Button>

            <AlertDialog>
                <AlertDialogTrigger>
                    <Button variant="destructive">
                        <Trash2 />
                    </Button>
                    <AlertConfirm @confirm="emit('deleted'); open = false"
                        :description="`This action will delete the table ${tableName}`" :action="deleteTable">
                    </AlertConfirm>
                </AlertDialogTrigger>
            </AlertDialog>

            <DialogFooter>
                <DialogClose as-child>
                    <Button type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="">
                    Save Changes
                </Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>